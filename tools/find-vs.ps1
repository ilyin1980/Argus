<#
.SYNOPSIS
    Write the path of the newest Visual Studio with the C++ toolset to a file.

.DESCRIPTION
    Called by build.bat. This lives in PowerShell rather than in the batch file
    because the search involves %ProgramFiles(x86)% — a path holding both spaces
    and parentheses — which batch cannot pass through `for /f` intact.

    Writes nothing when no suitable installation exists, so the caller can test
    for the file rather than parse an error.

.PARAMETER OutFile
    Where to write the installation path.
#>
[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutFile)

$ErrorActionPreference = 'SilentlyContinue'

$candidates = @(
    (Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'),
    (Join-Path $env:ProgramFiles        'Microsoft Visual Studio\Installer\vswhere.exe')
)
$vswhere = $candidates | Where-Object { $_ -and (Test-Path $_) } | Select-Object -First 1
if (-not $vswhere) { exit 1 }

$path = & $vswhere -latest -products * `
                   -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
                   -property installationPath
if (-not $path) { exit 1 }

# No BOM: the caller reads this with `set /p`, which would otherwise take the
# byte-order mark as part of the path.
[System.IO.File]::WriteAllText($OutFile, ($path | Select-Object -First 1).Trim(),
                               (New-Object System.Text.UTF8Encoding($false)))
