# ASIO4KRNL

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

A lightweight Win32 application located in `src/gui` provides control over the driver settings. It lets you select the input/output devices, choose buffer size and sample rate and toggle the low latency mode. Real‑time status information such as estimated latency and buffer underruns is refreshed every second. The application also exposes a tray icon and a start menu shortcut when installed. When no compatible device is found, the GUI warns the user and disables the selection boxes.

The GUI now groups related settings into clear sections and includes tooltips for every control. A "Restore Defaults" button resets the configuration. The tray icon menu allows quick toggling of low‑latency mode or reopening the window.

A WiX toolset script in the `installer` folder builds an MSI package that installs the driver, GUI and supporting files. It now creates desktop/start menu shortcuts, supports clean uninstallation and contains placeholders for certificate management and signing. On first install the GUI is launched with a simple setup wizard guiding new users.


## Beta Notes

Version 0.2.0 introduces functional ring buffer allocation and a simple ping‑pong
buffer processor. Crash logging remains enabled. Log files are written to
`C:\ProgramData\ASIO4Krnl\logs` for the driver and `%ProgramData%\ASIO4Krnl\logs`
for the GUI. A beta warning is shown on first launch of the control panel.

Run `installer/package.ps1` to build the signed MSI and package everything into
`ASIO4Krnl-Beta.zip` for distribution.
