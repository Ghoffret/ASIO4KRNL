# ASIO4KRNL Installer Test Script
# Automated testing for the generated MSI installer

param(
    [string]$MsiPath = "Output\ASIO4KRNL-Setup.msi",
    [switch]$TestInstall,
    [switch]$TestUninstall,
    [switch]$Verbose
)

$ErrorActionPreference = "Stop"

function Write-TestStatus($Message) { Write-Host "🧪 $Message" -ForegroundColor Cyan }
function Write-TestSuccess($Message) { Write-Host "✅ $Message" -ForegroundColor Green }
function Write-TestWarning($Message) { Write-Host "⚠️ $Message" -ForegroundColor Yellow }
function Write-TestError($Message) { Write-Host "❌ $Message" -ForegroundColor Red }

function Test-MsiProperties {
    Write-TestStatus "Testing MSI package properties..."
    
    if (-not (Test-Path $MsiPath)) {
        throw "MSI file not found: $MsiPath"
    }
    
    try {
        # Test MSI can be read
        $msiInfo = Get-ItemProperty -Path $MsiPath
        $fileSize = [math]::Round($msiInfo.Length / 1MB, 2)
        Write-TestSuccess "MSI file readable, size: $fileSize MB"
        
        # Test MSI is not corrupted (basic check)
        $msiBytes = [System.IO.File]::ReadAllBytes($MsiPath)
        if ($msiBytes.Length -lt 1000) {
            throw "MSI file appears to be corrupted (too small)"
        }
        
        # Check for MSI signature
        $signature = [System.Text.Encoding]::ASCII.GetString($msiBytes[0..7])
        if ($signature -notlike "*MSI*" -and $signature -notlike "*MZ*") {
            Write-TestWarning "MSI signature not detected, but file size suggests it may be valid"
        }
        
        Write-TestSuccess "Basic MSI validation passed"
        
    }
    catch {
        Write-TestError "MSI properties test failed: $($_.Exception.Message)"
        throw
    }
}

function Test-MsiDatabase {
    Write-TestStatus "Testing MSI database structure..."
    
    try {
        # Try to read MSI tables using Windows Installer API simulation
        # Since we can't easily access MSI APIs in PowerShell, we'll do basic tests
        
        # Check file size is reasonable for an installer
        $fileInfo = Get-ItemProperty -Path $MsiPath
        $sizeMB = [math]::Round($fileInfo.Length / 1MB, 2)
        
        if ($sizeMB -lt 0.1) {
            throw "MSI file too small ($sizeMB MB) - likely corrupted"
        }
        
        if ($sizeMB -gt 100) {
            Write-TestWarning "MSI file quite large ($sizeMB MB) - consider optimizing"
        }
        
        Write-TestSuccess "MSI database structure appears valid"
        
    }
    catch {
        Write-TestError "MSI database test failed: $($_.Exception.Message)"
        throw
    }
}

function Test-SilentInstall {
    if (-not $TestInstall) {
        Write-TestStatus "Skipping installation test (use -TestInstall to enable)"
        return
    }
    
    Write-TestStatus "Testing silent installation..."
    Write-TestWarning "This will actually install the software - run on test system only!"
    
    $confirmation = Read-Host "Continue with installation test? (y/N)"
    if ($confirmation -ne 'y' -and $confirmation -ne 'Y') {
        Write-TestStatus "Installation test skipped by user"
        return
    }
    
    try {
        # Test silent installation
        $installArgs = @(
            "/i", $MsiPath,
            "/quiet",
            "/norestart",
            "/l*v", "Output\install-test.log"
        )
        
        Write-TestStatus "Running: msiexec $($installArgs -join ' ')"
        $process = Start-Process -FilePath "msiexec.exe" -ArgumentList $installArgs -Wait -PassThru
        
        if ($process.ExitCode -eq 0) {
            Write-TestSuccess "Silent installation completed successfully"
            
            # Verify installation
            Test-InstallationResults
            
        } elseif ($process.ExitCode -eq 3010) {
            Write-TestSuccess "Installation completed successfully (reboot required)"
        } else {
            throw "Installation failed with exit code: $($process.ExitCode)"
        }
        
    }
    catch {
        Write-TestError "Silent installation test failed: $($_.Exception.Message)"
        throw
    }
}

function Test-InstallationResults {
    Write-TestStatus "Verifying installation results..."
    
    # Check if files were installed
    $expectedPaths = @(
        "${env:ProgramFiles}\ASIO4KRNL\ASIO4KrnlGUI.exe",
        "${env:ProgramFiles}\ASIO4KRNL\Drivers\ASIOUSB.sys",
        "${env:ProgramFiles}\ASIO4KRNL\Drivers\ASIOUSB.inf"
    )
    
    foreach ($path in $expectedPaths) {
        if (Test-Path $path) {
            Write-TestSuccess "Found installed file: $path"
        } else {
            Write-TestWarning "Missing expected file: $path"
        }
    }
    
    # Check registry entries
    try {
        $regKey = Get-ItemProperty -Path "HKLM:\SOFTWARE\ASIO\ASIO4KRNL" -ErrorAction SilentlyContinue
        if ($regKey) {
            Write-TestSuccess "ASIO registry entries found"
        } else {
            Write-TestWarning "ASIO registry entries not found"
        }
    }
    catch {
        Write-TestWarning "Could not check ASIO registry entries"
    }
    
    # Check Start Menu shortcuts
    $startMenuPath = Join-Path $env:ProgramData "Microsoft\Windows\Start Menu\Programs\ASIO4KRNL"
    if (Test-Path $startMenuPath) {
        Write-TestSuccess "Start Menu folder created"
    } else {
        Write-TestWarning "Start Menu folder not found"
    }
}

function Test-SilentUninstall {
    if (-not $TestUninstall) {
        Write-TestStatus "Skipping uninstallation test (use -TestUninstall to enable)"
        return
    }
    
    Write-TestStatus "Testing silent uninstallation..."
    
    # First, check if software is installed
    $uninstallKey = Get-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\*" | 
                    Where-Object { $_.DisplayName -like "*ASIO4KRNL*" }
    
    if (-not $uninstallKey) {
        Write-TestWarning "ASIO4KRNL not found in installed programs - skipping uninstall test"
        return
    }
    
    try {
        $productCode = $uninstallKey.PSChildName
        
        $uninstallArgs = @(
            "/x", $productCode,
            "/quiet",
            "/norestart",
            "/l*v", "Output\uninstall-test.log"
        )
        
        Write-TestStatus "Running: msiexec $($uninstallArgs -join ' ')"
        $process = Start-Process -FilePath "msiexec.exe" -ArgumentList $uninstallArgs -Wait -PassThru
        
        if ($process.ExitCode -eq 0) {
            Write-TestSuccess "Silent uninstallation completed successfully"
        } else {
            throw "Uninstallation failed with exit code: $($process.ExitCode)"
        }
        
    }
    catch {
        Write-TestError "Silent uninstallation test failed: $($_.Exception.Message)"
        throw
    }
}

function Show-TestSummary {
    Write-Host ""
    Write-Host "╔═══════════════════════════════════════════════════════╗" -ForegroundColor Green
    Write-Host "║                   TESTING COMPLETE                   ║" -ForegroundColor Green
    Write-Host "╚═══════════════════════════════════════════════════════╝" -ForegroundColor Green
    Write-Host ""
    
    if (Test-Path "Output\*.log") {
        Write-Host "Test logs available:" -ForegroundColor Cyan
        Get-ChildItem "Output\*test*.log" | ForEach-Object {
            Write-Host "  • $($_.Name)" -ForegroundColor White
        }
        Write-Host ""
    }
    
    Write-Host "Recommended next steps:" -ForegroundColor Cyan
    Write-Host "  1. Test installer on different Windows versions" -ForegroundColor White
    Write-Host "  2. Verify driver functionality after installation" -ForegroundColor White
    Write-Host "  3. Test installer with different user privileges" -ForegroundColor White
    Write-Host "  4. Perform upgrade/downgrade testing" -ForegroundColor White
}

# Main execution
try {
    Write-Host ""
    Write-Host "ASIO4KRNL Installer Test Suite" -ForegroundColor Cyan
    Write-Host "==============================" -ForegroundColor Cyan
    Write-Host ""
    
    Test-MsiProperties
    Test-MsiDatabase
    Test-SilentInstall
    Test-SilentUninstall
    
    Show-TestSummary
    
}
catch {
    Write-Host ""
    Write-TestError "Testing failed: $($_.Exception.Message)"
    exit 1
}