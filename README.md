# ASIO4KRNL

This repository contains a minimal skeleton of a KMDF-based audio driver for Windows. The goal is to create a native kernel driver that exposes an ASIO-compatible interface for low-latency audio using USB Audio Class devices (such as the Behringer UMC22/UM2).

The code is intentionally simplified and does **not** implement a full audio stack. It provides placeholders for key functionality including USB interface enumeration, stream format negotiation, ASIO buffer management and sample clock synchronisation.

## Building

The project requires the Windows Driver Kit (WDK) and should be built with Visual Studio. No build scripts are provided in this template.

## Files

- `src/driver/Driver.cpp` – main driver source with KMDF event callbacks and placeholder implementations.
- `src/driver/Driver.h` – declarations for the driver callbacks and modular functions.
- `src/driver/ASIOUSB.inf` – minimal INF file for installing the driver.

This driver is **not** ready for production use but serves as a starting point for further development.

