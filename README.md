# ASIO4KRNL
<img width="256" height="256" alt="AppIcon" src="https://github.com/user-attachments/assets/32ef8148-e104-4604-a97b-7c51cb1a4b2d" />

This repository contains a minimal skeleton of a KMDF-based audio driver for Windows. The goal is to create a native kernel driver that exposes an ASIO-compatible interface for low-latency audio using USB Audio Class devices (such as the Behringer UMC22/UM2).

This project was made with the help of **CODEX**.

The code is intentionally simplified and does **not** implement a full audio stack. It provides placeholders for key functionality including USB interface enumeration, stream format negotiation, ASIO buffer management and sample clock synchronisation. The newly added **KSAudioEngine** class demonstrates a clean approach to interfacing directly with the Windows Kernel Streaming subsystem without relying on ASIO4ALL-style wrappers.

## Building

Ensure you have Visual Studio and the Windows Driver Kit (WDK) installed. Run `setup.bat` from a Developer Command Prompt to build and optionally install the driver.
To actually install the driver, launch the script as **Administrator** so that `devcon` or `pnputil` can copy the driver package.

## Files

- `src/driver/Driver.cpp` – main driver source with KMDF event callbacks and placeholder implementations.
- `src/driver/Driver.h` – declarations for the driver callbacks and modular functions.
- `src/driver/ASIOUSB.inf` – minimal INF file for installing the driver.
- `setup.bat` – builds the project and installs the driver when run as Administrator.
- `src/driver/ASIOUSB.vcxproj` – Visual Studio project file for the kernel driver.
- `src/driver/ASIOInterface.cpp` – stub ASIO interface exported by the driver.
- `src/driver/KSAudioEngine.*` – core audio engine interfacing directly with
  Windows Kernel Streaming.

This driver is **not** ready for production use but serves as a starting point for further development.

## ASIO4KRNL vs ASIO4ALL

ASIO4ALL wraps existing Windows audio APIs in user mode, which can introduce additional latency and jitter. **ASIO4KRNL** aims to run entirely in the kernel as a KMDF driver. By interfacing directly with USB Audio Class devices, it provides lower latency, tighter timing control, and a cleaner integration surface than a user‑mode wrapper.


## Configuration GUI and Installer

A lightweight Win32 application located in `src/gui` provides control over the driver settings. The enhanced GUI features a modern design with improved usability and visual appeal:

### Enhanced GUI Features
- **Modern Design**: Clean layout with proper spacing, organized into logical groups
- **Theme Support**: Light and dark modes with consistent color schemes  
- **Enhanced Controls**: Dropdown menus for buffer sizes and sample rates with smart defaults
- **Input Validation**: Real-time validation with user-friendly error messages
- **Settings Testing**: Preview configurations before applying them permanently
- **Responsive Layout**: Window resizing support with proper control scaling
- **Detailed Tooltips**: Context-sensitive help for all controls
- **Status Monitoring**: Real-time latency calculation and performance feedback

### Key Improvements
The GUI now provides a significantly better user experience with:
- Organized sections for Audio Devices, Audio Settings, Driver Status, and Controls
- Dropdown selections for buffer sizes (64-2048 frames) and sample rates (44.1-192 kHz)
- Theme toggle button for switching between light and dark visual modes
- Test Settings button to validate configurations without permanent changes
- Enhanced system tray integration with theme switching support
- Comprehensive error handling with clear, actionable messages
- Responsive design that adapts to different window sizes

Real‑time status information includes estimated latency, buffer underruns, and driver state with color-coded feedback (green=optimal, yellow=warning, red=error). The application also exposes an enhanced tray icon with quick access to settings and theme switching. When no compatible device is found, the GUI provides detailed guidance for troubleshooting.

A WiX toolset script in the `installer` folder builds an MSI package that installs the driver, GUI and supporting files. It now creates desktop/start menu shortcuts, supports clean uninstallation and contains placeholders for certificate management and signing. On first install the GUI is launched with a simple setup wizard guiding new users.


## Beta Notes

Version 0.2.0 introduces functional ring buffer allocation and a simple ping‑pong
buffer processor. Crash logging remains enabled. Log files are written to
`C:\ProgramData\ASIO4Krnl\logs` for the driver and `%ProgramData%\ASIO4Krnl\logs`
for the GUI. A beta warning is shown on first launch of the control panel.

Run `installer/package.ps1` to build the signed MSI and package everything into
`ASIO4Krnl-Beta.zip` for distribution.
