<#
.SYNOPSIS
    Download everything Argus needs to build on Windows.

.DESCRIPTION
    Nothing here is committed to the repository: the runtimes are large, the
    model weights are larger, and both come from upstream releases that should
    be fetched rather than vendored. This script puts them where CMake looks.

    Everything lands under third_party/ and models/, both git-ignored. Re-running
    is cheap: anything already present is left alone unless -Force is given.

.PARAMETER Force
    Re-download even when a dependency is already in place.

.PARAMETER SkipModels
    Leave out the ONNX weights (about 70 MB). The duplicate finder works without
    them; object search does not.
#>
[CmdletBinding()]
param(
    [switch]$Force,
    [switch]$SkipModels
)

$ErrorActionPreference = 'Stop'
$ProgressPreference    = 'SilentlyContinue'

$root    = Split-Path -Parent $PSScriptRoot
$vendor  = Join-Path $root 'third_party'
$models  = Join-Path $root 'models'
$cache   = Join-Path $vendor '_downloads'

# Versions are pinned deliberately. The DirectML build of ONNX Runtime lags the
# GitHub releases and only ships through NuGet, and OpenCV 5 changes APIs, so
# neither "latest" is the right answer.
$ortVersion      = '1.24.4'
$directMlVersion = '1.15.4'
$openCvVersion   = '4.14.0'
$modelRelease    = 'v1.0.0'

New-Item -ItemType Directory -Force -Path $vendor, $models, $cache | Out-Null

function Get-File {
    param([string]$Url, [string]$Path)
    if ((Test-Path $Path) -and -not $Force) {
        Write-Host ("  cached  {0}" -f (Split-Path $Path -Leaf))
        return
    }
    Write-Host ("  fetch   {0}" -f (Split-Path $Path -Leaf))
    Invoke-WebRequest -Uri $Url -OutFile $Path -TimeoutSec 1800
}

function Expand-Nupkg {
    param([string]$Nupkg, [string]$Destination)
    $zip = [IO.Path]::ChangeExtension($Nupkg, '.zip')
    Copy-Item $Nupkg $zip -Force
    if (Test-Path $Destination) { Remove-Item $Destination -Recurse -Force }
    Expand-Archive $zip -DestinationPath $Destination -Force
}

# --- ONNX Runtime with DirectML, plus DirectML itself ------------------------
$ortRoot = Join-Path $vendor 'onnxruntime'
if ((Test-Path (Join-Path $ortRoot 'include\onnxruntime_cxx_api.h')) -and -not $Force) {
    Write-Host 'ONNX Runtime: already in place'
} else {
    Write-Host 'ONNX Runtime + DirectML:'
    $ortPkg = Join-Path $cache "onnxruntime.directml.$ortVersion.nupkg"
    $dmlPkg = Join-Path $cache "directml.$directMlVersion.nupkg"
    Get-File "https://api.nuget.org/v3-flatcontainer/microsoft.ml.onnxruntime.directml/$ortVersion/microsoft.ml.onnxruntime.directml.$ortVersion.nupkg" $ortPkg
    Get-File "https://api.nuget.org/v3-flatcontainer/microsoft.ai.directml/$directMlVersion/microsoft.ai.directml.$directMlVersion.nupkg" $dmlPkg

    $ortTmp = Join-Path $cache 'ort-extract'
    $dmlTmp = Join-Path $cache 'dml-extract'
    Expand-Nupkg $ortPkg $ortTmp
    Expand-Nupkg $dmlPkg $dmlTmp

    New-Item -ItemType Directory -Force -Path (Join-Path $ortRoot 'include'),
                                              (Join-Path $ortRoot 'lib'),
                                              (Join-Path $ortRoot 'bin') | Out-Null
    Copy-Item (Join-Path $ortTmp 'build\native\include\*') (Join-Path $ortRoot 'include') -Force
    Copy-Item (Join-Path $ortTmp 'runtimes\win-x64\native\onnxruntime.lib') (Join-Path $ortRoot 'lib') -Force
    Copy-Item (Join-Path $ortTmp 'runtimes\win-x64\native\*.dll') (Join-Path $ortRoot 'bin') -Force
    Copy-Item (Join-Path $dmlTmp 'bin\x64-win\DirectML.dll') (Join-Path $ortRoot 'bin') -Force
    Copy-Item (Join-Path $dmlTmp 'include\*') (Join-Path $ortRoot 'include') -Force -ErrorAction SilentlyContinue
    Write-Host "  -> $ortRoot"
}

# --- OpenCV ------------------------------------------------------------------
$openCvDir = Join-Path $vendor 'opencv'
if ((Test-Path (Join-Path $openCvDir 'build\OpenCVConfig.cmake')) -and -not $Force) {
    Write-Host 'OpenCV: already in place'
} else {
    Write-Host "OpenCV $openCvVersion :"
    $exe = Join-Path $cache "opencv-$openCvVersion-windows.exe"
    Get-File "https://github.com/opencv/opencv/releases/download/$openCvVersion/opencv-$openCvVersion-windows.exe" $exe

    # The installer is a 7-Zip self-extracting archive; -o -y only unpacks it.
    $sevenZip = @("$env:ProgramFiles\7-Zip\7z.exe", "${env:ProgramFiles(x86)}\7-Zip\7z.exe") |
                Where-Object { Test-Path $_ } | Select-Object -First 1
    if ($sevenZip) {
        & $sevenZip x $exe "-o$vendor" -y | Out-Null
    } else {
        & $exe "-o$vendor" -y
        Start-Sleep -Seconds 5
    }
    Write-Host "  -> $openCvDir"
}

# --- model weights -----------------------------------------------------------
if ($SkipModels) {
    Write-Host 'Models: skipped'
} else {
    Write-Host 'Models:'
    # From release v1.0.0 on purpose: later releases ship a fused
    # extractor+matcher pipeline, which forces re-extraction of every asset on
    # every query. We need the two halves separately.
    $wanted = @('disk.onnx', 'disk_lightglue_fused_fp16.onnx', 'disk_lightglue_fused_cpu.onnx')
    foreach ($m in $wanted) {
        Get-File "https://github.com/fabio-sim/LightGlue-ONNX/releases/download/$modelRelease/$m" (Join-Path $models $m)
    }
    Write-Host "  -> $models"
}

Write-Host ''
Write-Host 'Done. Build with: tools\build.bat'
