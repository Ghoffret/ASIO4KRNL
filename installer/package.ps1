# ASIO4KRNL Installer Build Script
# For developers only - end users should download pre-built installer

Write-Host "ASIO4KRNL Installer Build" -ForegroundColor Cyan
Write-Host "This script is for developers building the installer package." -ForegroundColor Yellow
Write-Host ""
Write-Host "END USERS: Download the pre-built installer from the releases page instead." -ForegroundColor Green
Write-Host ""

# Check for WiX Toolset
$WixPath = Get-Command "candle.exe" -ErrorAction SilentlyContinue
if (!$WixPath) {
    Write-Error "WiX Toolset not found. Install WiX Toolset v3.11+ for building."
}

$OutputDir = "Output"
if (!(Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

$WxsFile = "ASIO4KrnlInstaller.wxs"
$WixObjFile = "Output\ASIO4KrnlInstaller.wixobj"
$OutputMsi = "Output\ASIO4KRNL-x64.msi"

Write-Host "Building installer..." -ForegroundColor Cyan
& "candle.exe" "-arch" "x64" "-out" $WixObjFile $WxsFile
if ($LASTEXITCODE -eq 0) {
    & "light.exe" "-ext" "WixUIExtension" "-out" $OutputMsi $WixObjFile
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ Installer created: $OutputMsi" -ForegroundColor Green
    }
}
