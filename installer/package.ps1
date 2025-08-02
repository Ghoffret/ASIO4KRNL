# Build MSI installer and package into zip
# Optimized version with better error handling
$ErrorActionPreference = 'Stop'
$wix = "c:\Program Files (x86)\WiX Toolset v3.11\bin"
$msi = Join-Path $PSScriptRoot "ASIO4Krnl-Optimized.msi"

Write-Host "Building optimized ASIO4Krnl installer..."

try {
    & "$wix\candle.exe" ASIO4KrnlInstaller.wxs -out installer.wixobj
    if ($LASTEXITCODE -ne 0) { throw "Candle compilation failed" }
    
    & "$wix\light.exe" installer.wixobj -o $msi
    if ($LASTEXITCODE -ne 0) { throw "Light linking failed" }
    
    $zip = Join-Path $PSScriptRoot "ASIO4Krnl-Beta-Optimized.zip"
    if(Test-Path $zip){ Remove-Item $zip }
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    [System.IO.Compression.ZipFile]::CreateFromDirectory($PSScriptRoot, $zip)
    
    Write-Host "Successfully created optimized package: $zip" -ForegroundColor Green
}
catch {
    Write-Error "Build failed: $_"
    exit 1
}
