<#
.SYNOPSIS
    Generate the API documentation, fetching Doxygen if it is not installed.

.DESCRIPTION
    Nothing is installed system-wide: a portable Doxygen is unpacked under
    third_party/doxygen, which is already ignored by git, exactly like the other
    vendored binaries.

.PARAMETER Output
    Where the html/ directory is written. Defaults to build/doc.

.PARAMETER Open
    Open the generated index page when it is done.
#>
[CmdletBinding()]
param(
    [string]$Output = "",
    [switch]$Open
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$version = "1.13.2"
$vendor = Join-Path $root "third_party\doxygen"

function Resolve-Doxygen {
    $installed = Get-Command doxygen -ErrorAction SilentlyContinue
    if ($installed) { return $installed.Source }

    $local = Join-Path $vendor "doxygen.exe"
    if (Test-Path $local) { return $local }

    $url = "https://www.doxygen.nl/files/doxygen-$version.windows.x64.bin.zip"
    $zip = Join-Path $env:TEMP "doxygen-$version.zip"
    Write-Host "Fetching Doxygen $version..."
    Invoke-WebRequest -Uri $url -OutFile $zip -UseBasicParsing
    New-Item -ItemType Directory -Force -Path $vendor | Out-Null
    Expand-Archive -Path $zip -DestinationPath $vendor -Force
    Remove-Item $zip -Force

    if (-not (Test-Path $local)) {
        throw "Doxygen was fetched but $local is missing; the archive layout may have changed."
    }
    return $local
}

$doxygen = Resolve-Doxygen
if (-not $Output) { $Output = Join-Path $root "build\doc" }
New-Item -ItemType Directory -Force -Path $Output | Out-Null

# Same trick the CMake target uses: include the checked-in Doxyfile and only
# redirect the output, so there is one description of the documentation.
$generated = Join-Path $env:TEMP "ImageWorker.Doxyfile"
# Written through .NET rather than Set-Content: Windows PowerShell writes a
# byte-order mark, and Doxygen reports the mark as an unknown character on
# line 1 and then ignores the file.
[IO.File]::WriteAllText($generated, "@INCLUDE = $root\Doxyfile`nOUTPUT_DIRECTORY = $Output`n")

Push-Location $root
try {
    & $doxygen $generated
    if ($LASTEXITCODE -ne 0) { throw "doxygen exited with $LASTEXITCODE" }
} finally {
    Pop-Location
}

$index = Join-Path $Output "html\index.html"
Write-Host "Documentation written to $index"
if ($Open -and (Test-Path $index)) { Start-Process $index }
