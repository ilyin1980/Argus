<#
.SYNOPSIS
    Builds a synthetic "gameplay" screenshot with a known asset planted in it.

.DESCRIPTION
    Regression fixture for the search pipeline. The target asset is drawn at a
    known position and scale over a gradient, a few hundred random blobs and a
    couple of distractor sprites, which is enough clutter to break any
    whole-image fingerprint while leaving local features intact.

    Because the ground truth is printed, a search result can be checked against
    it rather than eyeballed.

.PARAMETER Target
    Asset to plant. Its filename is the correct answer.

.PARAMETER Distractors
    Other sprites drawn into the frame. These are genuinely present, so finding
    them is correct behaviour, not a false positive.

.PARAMETER OutDir
    Where screenshot.png and scaled.png are written.

.EXAMPLE
    .\tools\make-test-screenshot.ps1 -OutDir D:\Argus\testdata
#>
param(
    [string]   $Target      = "<path to a sprite with an alpha channel>",
    [string[]] $Distractors = @(
        "<path to a sprite with an alpha channel>",
        "<path to a sprite with an alpha channel>"),
    [string]   $OutDir      = "D:\Argus\testdata",
    [int]      $Width       = 1280,
    [int]      $Height      = 720,
    [double]   $Scale       = 0.62,
    [int]      $PlaceX      = 720,
    [int]      $PlaceY      = 330,
    [int]      $Seed        = 42
)

Add-Type -AssemblyName System.Drawing
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$bmp = New-Object System.Drawing.Bitmap($Width, $Height)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.SmoothingMode = 'AntiAlias'
$g.InterpolationMode = 'HighQualityBicubic'

$brush = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
    (New-Object System.Drawing.Point(0, 0)),
    (New-Object System.Drawing.Point($Width, $Height)),
    [System.Drawing.Color]::FromArgb(255, 40, 70, 110),
    [System.Drawing.Color]::FromArgb(255, 150, 90, 60))
$g.FillRectangle($brush, 0, 0, $Width, $Height)

# Fixed seed: the fixture must be identical on every run, or a change in search
# results cannot be attributed to a change in the code.
$rnd = New-Object System.Random $Seed
for ($i = 0; $i -lt 400; $i++) {
    $c = [System.Drawing.Color]::FromArgb(90, $rnd.Next(255), $rnd.Next(255), $rnd.Next(255))
    $b = New-Object System.Drawing.SolidBrush($c)
    $g.FillEllipse($b, $rnd.Next($Width), $rnd.Next($Height), $rnd.Next(10, 70), $rnd.Next(10, 70))
    $b.Dispose()
}

$i = 0
foreach ($d in $Distractors) {
    if (-not (Test-Path $d)) { Write-Warning "distractor missing: $d"; continue }
    $di = [System.Drawing.Image]::FromFile($d)
    $g.DrawImage($di, (60 + $i * 300), 40, 200, 200)
    $di.Dispose()
    $i++
}

if (-not (Test-Path $Target)) { throw "target missing: $Target" }
$ti = [System.Drawing.Image]::FromFile($Target)
$w = [int]($ti.Width * $Scale)
$h = [int]($ti.Height * $Scale)
$g.DrawImage($ti, $PlaceX, $PlaceY, $w, $h)

$g.Dispose()
$bmp.Save((Join-Path $OutDir "screenshot.png"), [System.Drawing.Imaging.ImageFormat]::Png)
$bmp.Dispose()

# A plain rescaled copy, as the easy control case.
$small = New-Object System.Drawing.Bitmap($ti, [int]($ti.Width * 0.6), [int]($ti.Height * 0.6))
$small.Save((Join-Path $OutDir "scaled.png"), [System.Drawing.Imaging.ImageFormat]::Png)
$small.Dispose()
$ti.Dispose()

Write-Host "ground truth : $(Split-Path $Target -Leaf) at $PlaceX,$PlaceY size ${w}x${h}"
Write-Host "written      : $OutDir\screenshot.png"
Write-Host "               $OutDir\scaled.png"
