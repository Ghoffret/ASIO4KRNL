ASIO4KRNL Installer Documentation
================================

This directory contains installer configurations for ASIO4KRNL, supporting both traditional setup (Inno Setup) and Windows Installer (MSI) formats.

## MSI Installer (Recommended)

The MSI installer provides the most comprehensive Windows installation experience with proper dependency management, versioning, and uninstallation support.

### Prerequisites for Building MSI:
- Visual Studio 2019 or later with Windows Driver Kit (WDK)
- WiX Toolset v3.11 or later (https://wixtoolset.org/releases/)
- Administrator privileges

### Building the MSI Installer:

#### Option 1: Using Build Scripts (Recommended)
```batch
# Using batch script
installer\build-msi.bat

# Using PowerShell script  
installer\build-msi.ps1
```

#### Option 2: Using MSBuild directly
```batch
# Build components first
msbuild src\driver\ASIOUSB.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild src\gui\ASIO4KrnlGUI.vcxproj /p:Configuration=Release /p:Platform=x64

# Build MSI installer
msbuild installer\ASIO4KrnlInstaller.wixproj /p:Configuration=Release
```

### MSI Features:
- User-friendly installation wizard with progress tracking
- Automatic driver installation using PnPUtil
- Registry entries for ASIO driver registration
- Start Menu and optional Desktop shortcuts
- Proper uninstallation through Windows Control Panel
- Version management for easy updates
- Administrator privilege verification

## Inno Setup Installer (Legacy)

The Inno Setup installer creates a self-extracting executable installer.

### Files:
- setup.iss: Inno Setup script configuration
- AppIcon.ico: Application icon for the installer

### To build the Inno Setup installer:
1. Install Inno Setup from https://jrsoftware.org/isinfo.php
2. Open setup.iss in Inno Setup Compiler
3. Compile the script to generate the installer executable

## Generated Files

### MSI Installer includes:
- ASIO driver files (ASIOUSB.sys, ASIOUSB.inf)
- GUI configuration utility (ASIO4KrnlGUI.exe)
- Documentation (README.md, LICENSE)
- Registry entries for ASIO registration
- Uninstaller for clean removal

### Installation Location:
- Default: `C:\Program Files\ASIO4KRNL\`
- Registry: `HKLM\SOFTWARE\ASIO\ASIO4KRNL`

## Version Information

Current version: 0.2.0-beta
Upgrade Code: {CDE4B0E2-2E5F-4C3F-95C8-1234567890AB}

## Troubleshooting

### Build Issues:
1. Ensure all prerequisites are installed
2. Run build scripts as Administrator
3. Check that all source files are built before running installer build
4. Verify WiX Toolset is in PATH

### Installation Issues:
1. Ensure you have Administrator privileges
2. Check Windows compatibility (Windows 7+ required)
3. Verify digital signature if driver signing is required
4. Check Windows Event Log for detailed error information

Make sure all required files are built and available in the appropriate directories before compiling the installer.

## Files in this directory:

- ASIO4KrnlInstaller.wxs: WiX source file for MSI generation
- ASIO4KrnlInstaller.wixproj: WiX project file for MSBuild integration
- build-msi.bat: Batch script for building MSI installer
- build-msi.ps1: PowerShell script for building MSI installer
- setup.iss: Inno Setup configuration (legacy)
- AppIcon.ico: Application icon
- README.txt: Original installer documentation