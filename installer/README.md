# ASIO4KRNL Installer

This directory contains the Windows Installer (MSI) package for ASIO4KRNL.

## For End Users

**[📥 Download the pre-built installer](../DOWNLOAD.md)** instead of building from source.

## For Developers

### Prerequisites

1. **WiX Toolset v3.11+** - Download from [wixtoolset.org](https://wixtoolset.org/)
2. **Visual Studio 2019/2022** - For building the driver and GUI components
3. **Windows 10/11 with Administrator privileges**

### Quick Build

#### Option 1: PowerShell (Recommended)
```powershell
cd installer
.\package.ps1
```

#### Option 2: Batch File
```cmd
cd installer
build.bat
```

### Build Output

The installer will be created as:
- `Output\ASIO4KRNL-Setup.msi` (Main installer package)
- Build logs in `Output\*.log`

### Testing

Run the automated test suite:
```powershell
.\test-installer.ps1 -TestInstall -TestUninstall
```

**Warning:** Testing will install/uninstall the actual software. Use on test systems only.

### File Structure

```
installer/
├── ASIO4KrnlInstaller.wxs    # Main WiX installer definition
├── package.ps1               # Enhanced PowerShell build script
├── build.bat                 # Batch file alternative
├── test-installer.ps1        # Automated testing script
├── AppIcon.ico               # Application icon
├── installer_banner.bmp      # Installer dialog banner
├── installer_header.bmp      # Installer header banner
├── README.txt                # Installer documentation
└── Output/                   # Build output directory
    ├── ASIO4KRNL-Setup.msi   # Final installer package
    └── *.log                 # Build and test logs
```

### Installer Features

The MSI installer includes:

- **Main Feature**: Core ASIO driver and GUI components
- **Documentation Feature**: User manuals and compatibility guides
- **Silent Installation**: Enterprise deployment support
- **Uninstall Support**: Complete removal with cleanup
- **Registry Integration**: ASIO driver registration
- **Shortcuts**: Start Menu and Desktop shortcuts
- **Administrator Privileges**: Required for driver installation

### Required Source Files

The installer expects the following files to be built:

```
src/driver/x64/Release/ASIOUSB.sys    # Kernel driver
src/gui/x64/Release/ASIO4KrnlGUI.exe  # Control panel GUI
src/driver/ASIOUSB.inf                # Driver information file
```

### Build Process Details

1. **Validation** - Check prerequisites and required files
2. **Compilation** - Process WiX source files with candle.exe
3. **Linking** - Generate MSI package with light.exe
4. **Testing** - Optional automated validation

### Troubleshooting

#### Common Issues

**"WiX Toolset not found"**
- Install WiX Toolset v3.11+ from wixtoolset.org
- Ensure candle.exe and light.exe are in your PATH

**"Missing required file"**
- Build the driver and GUI projects first
- Ensure all source files are present
- Check file paths in the WiX source

**"MSI validation failed"**
- Check build logs in Output directory
- Verify all components are properly defined
- Ensure file permissions are correct

#### Build Logs

Check these files for detailed error information:
- `Output\candle.log` - WiX compilation log
- `Output\light.log` - MSI linking log
- `Output\build.log` - Overall build log

### Advanced Options

#### Silent Build
```powershell
.\package.ps1 -Silent
```

#### Clean Build
```powershell
.\package.ps1 -Clean
```

#### Test Only
```powershell
.\package.ps1 -Test
```

### Distribution

After building:

1. **Test** the installer on clean Windows systems
2. **Sign** the MSI for trusted installation
3. **Upload** to release distribution platform
4. **Update** download documentation

## Code Signing (Future)

The installer is prepared for code signing with placeholders for:
- Certificate management (`CERTMGR` property)
- Signing tool (`SIGNTOOL` property)

Update the WiX file with actual certificate details before distribution.

## Support

For build issues:
1. Check this documentation
2. Review build logs in Output directory  
3. Create issue on GitHub with log files attached