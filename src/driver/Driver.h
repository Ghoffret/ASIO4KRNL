#pragma once

#include <ntddk.h>
#include <wdf.h>

extern "C" {
DRIVER_INITIALIZE DriverEntry;
}

NTSTATUS
ASIOEvtDeviceAdd(
    _In_ WDFDRIVER Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

EVT_WDF_DRIVER_UNLOAD ASIOEvtDriverUnload;
EVT_WDF_DEVICE_PREPARE_HARDWARE ASIOEvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE ASIOEvtDeviceReleaseHardware;

// Placeholder functions for modular functionality
NTSTATUS EnumerateUsbInterfaces(_In_ WDFDEVICE Device);
NTSTATUS NegotiateStreamFormat(_In_ WDFDEVICE Device);
NTSTATUS SetupAsioBuffers(_In_ WDFDEVICE Device);
NTSTATUS SyncSampleClock(_In_ WDFDEVICE Device);


