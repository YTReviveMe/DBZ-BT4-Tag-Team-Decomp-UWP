Add-Type -AssemblyName System.Drawing

$assets = Join-Path $PSScriptRoot 'assets'
$fonts = [Drawing.Text.PrivateFontCollection]::new()
$fonts.AddFontFile((Resolve-Path (Join-Path $PSScriptRoot '../third_party/fonts/anton/Anton-Regular.ttf')).Path)
$display = $fonts.Families[0]
$navy = [Drawing.Color]::FromArgb(16, 27, 50)
$white = [Drawing.Color]::FromArgb(245, 249, 255)
$cyan = [Drawing.Color]::FromArgb(74, 205, 255)
$gold = [Drawing.Color]::FromArgb(255, 198, 59)

function New-Canvas([int]$width, [int]$height) {
    $bitmap = [Drawing.Bitmap]::new($width, $height, [Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $graphics = [Drawing.Graphics]::FromImage($bitmap)
    $graphics.SmoothingMode = [Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $graphics.TextRenderingHint = [Drawing.Text.TextRenderingHint]::AntiAliasGridFit
    $graphics.Clear($navy)
    return @($bitmap, $graphics)
}

function Draw-CenteredText($graphics, [string]$text, [float]$size, [float]$top,
                           [float]$width, [Drawing.Color]$color) {
    $font = [Drawing.Font]::new($display, $size, [Drawing.FontStyle]::Regular, [Drawing.GraphicsUnit]::Pixel)
    $brush = [Drawing.SolidBrush]::new($color)
    $format = [Drawing.StringFormat]::new()
    try {
        $format.Alignment = [Drawing.StringAlignment]::Center
        $format.LineAlignment = [Drawing.StringAlignment]::Near
        $graphics.DrawString($text, $font, $brush, [Drawing.RectangleF]::new(0, $top, $width, $size * 1.5), $format)
    } finally {
        $format.Dispose()
        $brush.Dispose()
        $font.Dispose()
    }
}

function Save-Canvas($canvas, [string]$name) {
    $canvas[1].Dispose()
    try {
        $canvas[0].Save((Join-Path $assets $name), [Drawing.Imaging.ImageFormat]::Png)
    } finally {
        $canvas[0].Dispose()
    }
}

foreach ($item in @(@('BT4Square150x150Logo.png', 150), @('BT4Square44x44Logo.png', 44), @('BT4StoreLogo.png', 50))) {
    $name = [string]$item[0]
    $side = [int]$item[1]
    $canvas = New-Canvas $side $side
    $g = $canvas[1]
    $rule = [Drawing.Pen]::new($cyan, [Math]::Max(1, $side / 60))
    try {
        $g.DrawLine($rule, $side * 0.16, $side * 0.18, $side * 0.84, $side * 0.18)
        Draw-CenteredText $g 'BT4' ($side * 0.43) ($side * 0.23) $side $gold
        if ($side -gt 50) {
            Draw-CenteredText $g 'TAG TEAM' ($side * 0.13) ($side * 0.74) $side $white
        } else {
            $g.DrawLine($rule, $side * 0.16, $side * 0.82, $side * 0.84, $side * 0.82)
        }
    } finally {
        $rule.Dispose()
    }
    Save-Canvas $canvas $name
}

$fonts.Dispose()
