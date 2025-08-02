# ASIO4KRNL Professional Installer Build Script
# Creates a complete MSI package for distribution

param(
    [string]$Configuration = "Release",
    [string]$Platform = "x64", 
    [switch]$Sign = $false,
    [switch]$Test = $false
)

$ErrorActionPreference = "Stop"

# Script configuration
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$InstallerDir = $PSScriptRoot
$OutputDir = Join-Path $InstallerDir "Output"
$Version = "1.0.0"

Write-Host "ASIO4KRNL Professional Installer Build Script" -ForegroundColor Cyan
Write-Host "Version: $Version" -ForegroundColor Green
Write-Host "Configuration: $Configuration" -ForegroundColor Green
Write-Host "Platform: $Platform" -ForegroundColor Green

# Ensure output directory exists
if (!(Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

# Check prerequisites
Write-Host "`nChecking prerequisites..." -ForegroundColor Yellow

# Check for WiX Toolset
$WixPath = Get-Command "candle.exe" -ErrorAction SilentlyContinue
if (!$WixPath) {
    Write-Error "WiX Toolset not found. Please install WiX Toolset v3.11 or later."
}
Write-Host "✓ WiX Toolset found: $($WixPath.Source)" -ForegroundColor Green

# Create installer
Write-Host "`nBuilding MSI installer..." -ForegroundColor Yellow

$WxsFile = Join-Path $InstallerDir "ASIO4KrnlInstaller.wxs"
$WixObjFile = Join-Path $OutputDir "ASIO4KrnlInstaller.wixobj"
$OutputMsi = Join-Path $OutputDir "ASIO4KRNL-$Version-$Platform.msi"

# Compile WiX source
Write-Host "Compiling WiX source..." -ForegroundColor Cyan
& "candle.exe" "-arch" $Platform.ToLower() "-out" $WixObjFile $WxsFile
if ($LASTEXITCODE -ne 0) {
    Write-Error "WiX compilation failed with exit code $LASTEXITCODE"
}

# Link MSI
Write-Host "Linking MSI package..." -ForegroundColor Cyan
& "light.exe" "-ext" "WixUIExtension" "-cultures:en-us" "-out" $OutputMsi $WixObjFile
if ($LASTEXITCODE -ne 0) {
    Write-Error "WiX linking failed with exit code $LASTEXITCODE"
}

Write-Host "✓ MSI package created: $OutputMsi" -ForegroundColor Green
Write-Host "`n✓ ASIO4KRNL installer build completed successfully!" -ForegroundColor Green
