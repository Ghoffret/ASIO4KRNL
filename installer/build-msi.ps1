# ASIO4KRNL MSI Installer Build Script (PowerShell)
# Requires: Visual Studio with WDK, WiX Toolset v3.11+

param(
    [string]$Configuration = "Release",
    [string]$Platform = "x64"
)

Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "ASIO4KRNL MSI Installer Build Script" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan

# Check for administrator privileges
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
if (-not $isAdmin) {
    Write-Host "ERROR: This script must be run as Administrator." -ForegroundColor Red
    Write-Host "Right-click PowerShell and select 'Run as administrator'" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

# Set paths
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = Split-Path -Parent $ScriptDir
$SrcDir = Join-Path $RootDir "src"
$InstallerDir = $ScriptDir

Write-Host "Script Directory: $ScriptDir" -ForegroundColor Green
Write-Host "Root Directory: $RootDir" -ForegroundColor Green
Write-Host "Source Directory: $SrcDir" -ForegroundColor Green
Write-Host "Configuration: $Configuration" -ForegroundColor Green
Write-Host "Platform: $Platform" -ForegroundColor Green

# Find MSBuild
$msbuild = Get-Command msbuild.exe -ErrorAction SilentlyContinue
if (-not $msbuild) {
    Write-Host "ERROR: MSBuild not found. Please install Visual Studio with Windows Driver Kit." -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}
Write-Host "Found MSBuild: $($msbuild.Source)" -ForegroundColor Green

# Check for WiX Toolset
$candle = Get-Command candle.exe -ErrorAction SilentlyContinue
if (-not $candle) {
    Write-Host "ERROR: WiX Toolset not found. Please install WiX Toolset v3.11 or later." -ForegroundColor Red
    Write-Host "Download from: https://wixtoolset.org/releases/" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}
Write-Host "Found WiX Toolset: $($candle.Source)" -ForegroundColor Green

Write-Host ""
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Building ASIO4KRNL Components" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan

# Build the driver
Write-Host "Building ASIO driver..." -ForegroundColor Yellow
$driverProject = Join-Path $SrcDir "driver\ASIOUSB.vcxproj"
& $msbuild.Source $driverProject /t:Build /p:Configuration=$Configuration /p:Platform=$Platform /nologo
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Driver build failed." -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit $LASTEXITCODE
}
Write-Host "Driver build completed successfully." -ForegroundColor Green

# Build the GUI application
Write-Host "Building GUI application..." -ForegroundColor Yellow
$guiProject = Join-Path $SrcDir "gui\ASIO4KrnlGUI.vcxproj"
& $msbuild.Source $guiProject /t:Build /p:Configuration=$Configuration /p:Platform=$Platform /nologo
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: GUI build failed." -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit $LASTEXITCODE
}
Write-Host "GUI build completed successfully." -ForegroundColor Green

Write-Host ""
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Building MSI Installer" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan

# Build the MSI installer
Write-Host "Building MSI installer..." -ForegroundColor Yellow
$wixProject = Join-Path $InstallerDir "ASIO4KrnlInstaller.wixproj"
& $msbuild.Source $wixProject /t:Build /p:Configuration=$Configuration /nologo
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: MSI build failed." -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Build Completed Successfully!" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan

# Check if MSI was created
$msiFile = Join-Path $InstallerDir "bin\$Configuration\ASIO4KRNL-$Configuration.msi"
if (Test-Path $msiFile) {
    $fileInfo = Get-Item $msiFile
    Write-Host "MSI installer created: $msiFile" -ForegroundColor Green
    Write-Host "File size: $($fileInfo.Length) bytes" -ForegroundColor Green
    Write-Host "Creation time: $($fileInfo.CreationTime)" -ForegroundColor Green
    
    $choice = Read-Host "Do you want to open the installer directory? (Y/N)"
    if ($choice -eq "Y" -or $choice -eq "y") {
        Start-Process explorer.exe -ArgumentList (Split-Path $msiFile)
    }
} else {
    Write-Host "WARNING: MSI file not found at expected location: $msiFile" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Build script completed." -ForegroundColor Green
Read-Host "Press Enter to exit"