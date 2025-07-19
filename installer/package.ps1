# Build MSI installer and package into zip
$ErrorActionPreference = 'Stop'
$wix = "c:\Program Files (x86)\WiX Toolset v3.11\bin"
$msi = Join-Path $PSScriptRoot "ASIO4Krnl.msi"

& "$wix\candle.exe" ASIO4KrnlInstaller.wxs -out installer.wixobj
& "$wix\light.exe" installer.wixobj -o $msi

$zip = Join-Path $PSScriptRoot "ASIO4Krnl-Beta.zip"
if(Test-Path $zip){ Remove-Item $zip }
Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory($PSScriptRoot, $zip)

Write-Host "Created $zip"
