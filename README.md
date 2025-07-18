# ASIO4KRNL

This repository contains a minimal skeleton of a KMDF-based audio driver for Windows. The goal is to create a native kernel driver that exposes an ASIO-compatible interface for low-latency audio using USB Audio Class devices (such as the Behringer UMC22/UM2).

The code is intentionally simplified and does **not** implement a full audio stack. It provides placeholders for key functionality including USB interface enumeration, stream format negotiation, ASIO buffer management and sample clock synchronisation.

## Building

Ensure you have Visual Studio and the Windows Driver Kit (WDK) installed. Run `setup.bat` from a Developer Command Prompt to build and optionally install the driver.

## Files

- `src/driver/Driver.cpp` – main driver source with KMDF event callbacks and placeholder implementations.  
- `src/driver/Driver.h` – declarations for the driver callbacks and modular functions.  
- `src/driver/ASIOUSB.inf` – minimal INF file for installing the driver.

This driver is **not** ready for production use but serves as a starting point for further development.

## ASIO4KRNL vs ASIO4ALL

ASIO4ALL wraps existing Windows audio APIs in user mode, which can introduce additional latency and jitter. **ASIO4KRNL** aims to run entirely in the kernel as a KMDF driver. By interfacing directly with USB Audio Class devices, it provides lower latency, tighter timing control, and a cleaner integration surface than a user‑mode wrapper.

