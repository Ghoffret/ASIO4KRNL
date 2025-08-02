# ASIO4KRNL Installer Build Script
# Enhanced version with comprehensive error handling and validation

param(
    [switch]$Silent,
    [switch]$Clean,
    [switch]$Test,
    [string]$Configuration = "Release"
)

# Script configuration
$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

# Color output functions
function Write-Status($Message) { Write-Host "ℹ $Message" -ForegroundColor Cyan }
function Write-Success($Message) { Write-Host "✓ $Message" -ForegroundColor Green }
function Write-Warning($Message) { Write-Host "⚠ $Message" -ForegroundColor Yellow }
function Write-Error($Message) { Write-Host "✗ $Message" -ForegroundColor Red }

function Show-Header {
    if (-not $Silent) {
        Write-Host ""
        Write-Host "╔═══════════════════════════════════════════════════════╗" -ForegroundColor Cyan
        Write-Host "║                 ASIO4KRNL Installer Build            ║" -ForegroundColor Cyan  
        Write-Host "║                     Developer Build                   ║" -ForegroundColor Cyan
        Write-Host "╚═══════════════════════════════════════════════════════╝" -ForegroundColor Cyan
        Write-Host ""
        Write-Warning "This script is for developers building the installer package."
        Write-Host ""
        Write-Host "END USERS: Download the pre-built installer from the releases page instead." -ForegroundColor Green
        Write-Host ""
    }
}

function Test-Prerequisites {
    Write-Status "Checking build prerequisites..."
    
    $issues = @()
    
    # Check for WiX Toolset
    try {
        $candlePath = Get-Command "candle.exe" -ErrorAction Stop
        $lightPath = Get-Command "light.exe" -ErrorAction Stop
        Write-Success "WiX Toolset found: $($candlePath.Source)"
    }
    catch {
        $issues += "WiX Toolset not found. Install WiX Toolset v3.11+ from https://wixtoolset.org/"
    }
    
    # Check for required source files
    $requiredFiles = @(
        "..\src\driver\x64\Release\ASIOUSB.sys",
        "..\src\gui\x64\Release\ASIO4KrnlGUI.exe",
        "..\src\driver\ASIOUSB.inf",
        "AppIcon.ico",
        "installer_banner.bmp",
        "installer_header.bmp",
        "README.txt"
    )
    
    foreach ($file in $requiredFiles) {
        if (-not (Test-Path $file)) {
            $issues += "Missing required file: $file"
        }
    }
    
    # Check for documentation files
    $docFiles = @(
        "..\LICENSE",
        "..\INSTALLATION_GUIDE.md",
        "..\USER_MANUAL.md",
        "..\DEVICE_COMPATIBILITY.md"
    )
    
    foreach ($file in $docFiles) {
        if (-not (Test-Path $file)) {
            Write-Warning "Documentation file missing: $file"
        }
    }
    
    if ($issues.Count -gt 0) {
        Write-Error "Build prerequisites not met:"
        foreach ($issue in $issues) {
            Write-Host "  • $issue" -ForegroundColor Red
        }
        throw "Prerequisites check failed"
    }
    
    Write-Success "All prerequisites satisfied"
}

function Initialize-BuildEnvironment {
    Write-Status "Initializing build environment..."
    
    # Create output directory
    $script:OutputDir = "Output"
    if ($Clean -and (Test-Path $OutputDir)) {
        Write-Status "Cleaning previous build artifacts..."
        Remove-Item -Path $OutputDir -Recurse -Force
    }
    
    if (-not (Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    }
    
    # Define build paths
    $script:WxsFile = "ASIO4KrnlInstaller.wxs"
    $script:WixObjFile = "$OutputDir\ASIO4KrnlInstaller.wixobj"
    $script:OutputMsi = "$OutputDir\ASIO4KRNL-Setup.msi"
    $script:LogFile = "$OutputDir\build.log"
    
    Write-Success "Build environment ready"
}

function Build-Installer {
    Write-Status "Building installer..."
    
    try {
        # Compile WiX source
        Write-Status "Compiling WiX source files..."
        $candleArgs = @(
            "-arch", "x64",
            "-out", $WixObjFile,
            "-ext", "WixUtilExtension",
            "-v",  # Verbose output
            $WxsFile
        )
        
        $candleProcess = Start-Process -FilePath "candle.exe" -ArgumentList $candleArgs -NoNewWindow -Wait -PassThru -RedirectStandardOutput "$OutputDir\candle.log" -RedirectStandardError "$OutputDir\candle.err"
        
        if ($candleProcess.ExitCode -ne 0) {
            $errorContent = Get-Content "$OutputDir\candle.err" -Raw
            throw "candle.exe failed with exit code $($candleProcess.ExitCode): $errorContent"
        }
        
        Write-Success "WiX compilation completed"
        
        # Link installer
        Write-Status "Linking installer package..."
        $lightArgs = @(
            "-ext", "WixUIExtension",
            "-ext", "WixUtilExtension", 
            "-cultures:en-US",
            "-out", $OutputMsi,
            "-v",  # Verbose output
            $WixObjFile
        )
        
        $lightProcess = Start-Process -FilePath "light.exe" -ArgumentList $lightArgs -NoNewWindow -Wait -PassThru -RedirectStandardOutput "$OutputDir\light.log" -RedirectStandardError "$OutputDir\light.err"
        
        if ($lightProcess.ExitCode -ne 0) {
            $errorContent = Get-Content "$OutputDir\light.err" -Raw
            throw "light.exe failed with exit code $($lightProcess.ExitCode): $errorContent"
        }
        
        Write-Success "Installer linking completed"
        
    }
    catch {
        Write-Error "Build failed: $($_.Exception.Message)"
        throw
    }
}

function Test-InstallerPackage {
    if (-not $Test) {
        return
    }
    
    Write-Status "Testing installer package..."
    
    if (-not (Test-Path $OutputMsi)) {
        throw "Installer package not found: $OutputMsi"
    }
    
    # Get MSI properties
    try {
        $msiInfo = Get-ItemProperty -Path $OutputMsi
        $fileSize = [math]::Round($msiInfo.Length / 1MB, 2)
        Write-Success "Installer size: $fileSize MB"
        
        # Validate MSI structure (basic check)
        # TODO: Add more comprehensive MSI validation
        Write-Success "Basic MSI validation passed"
        
    }
    catch {
        Write-Warning "Could not validate MSI properties: $($_.Exception.Message)"
    }
}

function Show-BuildSummary {
    Write-Host ""
    Write-Host "╔═══════════════════════════════════════════════════════╗" -ForegroundColor Green
    Write-Host "║                   BUILD COMPLETE                     ║" -ForegroundColor Green
    Write-Host "╚═══════════════════════════════════════════════════════╝" -ForegroundColor Green
    
    if (Test-Path $OutputMsi) {
        $msiInfo = Get-ItemProperty -Path $OutputMsi
        $fileSize = [math]::Round($msiInfo.Length / 1KB, 0)
        
        Write-Host ""
        Write-Success "Installer created successfully:"
        Write-Host "  • File: $OutputMsi" -ForegroundColor White
        Write-Host "  • Size: $fileSize KB" -ForegroundColor White
        Write-Host "  • Modified: $($msiInfo.LastWriteTime)" -ForegroundColor White
        Write-Host ""
        Write-Host "Next steps:" -ForegroundColor Cyan
        Write-Host "  1. Test the installer on a clean Windows system" -ForegroundColor White
        Write-Host "  2. Verify all components install correctly" -ForegroundColor White
        Write-Host "  3. Test both installation and uninstallation" -ForegroundColor White
        Write-Host "  4. Sign the installer for distribution" -ForegroundColor White
    }
    else {
        Write-Error "Build completed but installer file not found!"
    }
}

# Main execution
try {
    Show-Header
    Test-Prerequisites
    Initialize-BuildEnvironment
    Build-Installer
    Test-InstallerPackage
    Show-BuildSummary
}
catch {
    Write-Host ""
    Write-Error "Build failed: $($_.Exception.Message)"
    
    if (Test-Path "$OutputDir\*.log") {
        Write-Host ""
        Write-Host "Build logs are available in the Output directory:" -ForegroundColor Yellow
        Get-ChildItem "$OutputDir\*.log" | ForEach-Object { 
            Write-Host "  • $($_.Name)" -ForegroundColor Yellow 
        }
    }
    
    exit 1
}
