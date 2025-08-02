# ASIO4KRNL Installation Guide

**Complete step-by-step installation instructions for Windows users**

## Before You Begin

### System Requirements
- **Operating System**: Windows 10 (64-bit) or Windows 11
- **Audio Interface**: USB Audio Class compatible device
- **Permissions**: Administrator access required for installation
- **Free Space**: Minimum 50MB available disk space

### Important Notes
- Close all audio applications before installation
- Disconnect and reconnect your audio interface after installation
- Backup any existing ASIO driver settings if needed

## Installation Process

### Step 1: Download ASIO4KRNL
1. Visit the official ASIO4KRNL releases page
2. Download the latest `ASIO4KRNL-Setup.msi` installer
3. Save the file to your Downloads folder or desktop

### Step 2: Run the Installer
1. **Right-click** on `ASIO4KRNL-Setup.msi`
2. Select **"Run as administrator"** from the context menu
3. Click **"Yes"** when Windows asks for permission

### Step 3: Installation Wizard
1. **Welcome Screen**: Click "Next" to begin installation
2. **License Agreement**: Read and accept the license terms
3. **Installation Folder**: Use default location or choose custom path
4. **Components**: Ensure all components are selected:
   - ASIO4KRNL Kernel Driver
   - ASIO4KRNL Control Panel
   - Documentation and User Guide
5. **Ready to Install**: Click "Install" to begin

### Step 4: Driver Installation
1. Windows may show security warnings - click **"Install anyway"**
2. The installer will automatically:
   - Copy driver files to system directories
   - Register the ASIO4KRNL driver with Windows
   - Install the control panel application
   - Create Start Menu shortcuts

### Step 5: Complete Installation
1. Click **"Finish"** when installation completes
2. **Restart your computer** if prompted
3. The ASIO4KRNL Control Panel will launch automatically

## Initial Configuration

### Step 1: Launch Control Panel
- **Start Menu**: Search for "ASIO4KRNL Settings"
- **Desktop**: Double-click the ASIO4KRNL icon
- **System Tray**: Right-click the ASIO4KRNL icon and select "Open Settings"

### Step 2: Configure Audio Devices
1. **Input Device**: Select your audio interface from the dropdown
2. **Output Device**: Usually the same as input device
3. If your device doesn't appear:
   - Disconnect and reconnect the USB cable
   - Click "Refresh Devices" if available
   - Check Windows Device Manager for driver issues

### Step 3: Set Buffer Size
1. **For beginners**: Start with 256 or 512 samples
2. **For low latency**: Try 128 or 64 samples (may require more CPU)
3. **For stability**: Use 512 or 1024 samples on older computers

### Step 4: Choose Sample Rate
1. **Most common**: 48,000 Hz (48 kHz)
2. **High quality**: 96,000 Hz or 192,000 Hz
3. **Compatibility**: 44,100 Hz for CD-quality audio

### Step 5: Test Settings
1. Click **"Test Settings"** to verify configuration
2. Look for green status indicators
3. If test fails, try larger buffer size or lower sample rate

## Configure Your DAW

### FL Studio
1. Open FL Studio
2. Go to **Options → Audio Settings**
3. Set **Device** to "ASIO4KRNL"
4. Set **Buffer length** to match your ASIO4KRNL buffer size
5. Click **"Show ASIO panel"** to open ASIO4KRNL settings

### REAPER
1. Open REAPER
2. Go to **Options → Preferences → Audio → Device**
3. Set **Audio system** to "ASIO"
4. Set **ASIO driver** to "ASIO4KRNL"
5. Click **"ASIO Configuration"** to adjust settings

### Ableton Live
1. Open Ableton Live
2. Go to **Options → Preferences → Audio**
3. Set **Driver Type** to "ASIO"
4. Set **Audio Device** to "ASIO4KRNL"
5. Adjust **Buffer Size** to match your ASIO4KRNL settings

### Pro Tools
1. Open Pro Tools
2. Go to **Setup → Hardware Setup**
3. Select "ASIO4KRNL" from the device list
4. Configure buffer size and sample rate as needed

### Other DAWs
Most professional audio software follows similar patterns:
1. Open audio/device preferences
2. Select ASIO as driver type
3. Choose ASIO4KRNL from available drivers
4. Match buffer and sample rate settings

## Verification and Testing

### Check Installation Success
1. **System Tray**: Look for ASIO4KRNL icon in bottom-right corner
2. **Device Manager**: Verify "ASIO4KRNL Audio Driver" appears under "Sound, video and game controllers"
3. **Control Panel**: Successfully opens and detects your audio interface

### Test Audio Performance
1. **Play audio** through your DAW or media player
2. **Monitor latency** in ASIO4KRNL control panel
3. **Check for dropouts** or clicks/pops
4. **Record audio** to test input functionality

### Optimize Performance
1. **Adjust buffer size** for best latency vs. stability balance
2. **Close unnecessary programs** to free up CPU resources
3. **Disable Windows audio enhancements** for your device
4. **Use high-performance power plan** when recording/producing

## Troubleshooting Common Issues

### Audio Interface Not Detected
**Problem**: Device doesn't appear in ASIO4KRNL settings
**Solutions**:
- Reconnect USB cable to different port
- Install manufacturer's official drivers first
- Check Windows Device Manager for hardware issues
- Try different USB cable

### High Latency or Audio Dropouts
**Problem**: Poor audio performance despite correct settings
**Solutions**:
- Increase buffer size to 512 or 1024 samples
- Close other audio applications
- Disable Windows audio effects
- Update audio interface firmware

### Installation Failed
**Problem**: Installer shows errors or won't complete
**Solutions**:
- Run installer as Administrator
- Temporarily disable antivirus software
- Clear Windows installer cache
- Download fresh installer copy

### DAW Doesn't See ASIO4KRNL
**Problem**: ASIO4KRNL not available in audio software
**Solutions**:
- Restart your DAW after installation
- Check DAW is running in same architecture (64-bit)
- Reinstall ASIO4KRNL if necessary
- Contact DAW manufacturer for ASIO support

## Uninstallation

### Remove ASIO4KRNL
1. Open **Windows Settings → Apps**
2. Search for "ASIO4KRNL"
3. Click the entry and select **"Uninstall"**
4. Follow the uninstaller prompts
5. **Restart** your computer when complete

### Clean Removal
If standard uninstall doesn't work:
1. Use Windows "Programs and Features"
2. Manually delete leftover files from Program Files
3. Clear registry entries (advanced users only)
4. Contact support for assistance

## Support and Next Steps

### Getting Help
- **User Manual**: [USER_MANUAL.md](USER_MANUAL.md) for detailed configuration
- **Device Compatibility**: [DEVICE_COMPATIBILITY.md](DEVICE_COMPATIBILITY.md) for hardware support
- **FAQ**: [FAQ.md](FAQ.md) for common questions
- **Technical Support**: Visit our support forum or contact documentation

### Optimize Your Setup
1. Read the complete [User Manual](USER_MANUAL.md)
2. Learn about advanced features and settings
3. Explore device-specific optimizations
4. Join the ASIO4KRNL user community

---

**Congratulations! You've successfully installed ASIO4KRNL and are ready to experience professional-grade audio performance on Windows.**