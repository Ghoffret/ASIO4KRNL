#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <wdfusb.h>

// Performance and buffer size constants
#define ASIO_BUFFER_SIZE_BYTES      4096
#define RING_BUFFER_SIZE_BYTES      16384
#define BUFFER_TIMER_PERIOD_MS      1
#define DEFAULT_SAMPLE_RATE         48000
#define DEFAULT_CHANNEL_COUNT       2
#define DEFAULT_BUFFER_FRAMES       256

// Pool tags for memory allocation tracking
#define TAG_ASIO_BUFFER    '4ksA'
#define TAG_RING_BUFFER    '4ksR'

extern "C" {
    DRIVER_INITIALIZE DriverEntry;
}

NTSTATUS
ASIOEvtDeviceAdd(
    _In_ WDFDRIVER Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

EVT_WDF_DRIVER_UNLOAD              ASIOEvtDriverUnload;
EVT_WDF_DEVICE_PREPARE_HARDWARE    ASIOEvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE    ASIOEvtDeviceReleaseHardware;

// Placeholder functions for modular functionality
NTSTATUS EnumerateUsbInterfaces(_In_ WDFDEVICE Device);
NTSTATUS NegotiateStreamFormat(_In_ WDFDEVICE Device);
NTSTATUS SetupAsioBuffers(_In_ WDFDEVICE Device);
NTSTATUS SyncSampleClock(_In_ WDFDEVICE Device);

// USB context storing opened pipes for streaming
typedef struct _USB_DEVICE_CONTEXT {
    WDFUSBDEVICE    UsbDevice;
    WDFUSBINTERFACE AudioInterface;
    WDFUSBPIPE      StreamInPipe;
    WDFUSBPIPE      StreamOutPipe;
} USB_DEVICE_CONTEXT, *PUSB_DEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USB_DEVICE_CONTEXT, UsbGetDeviceContext);

// Basic representation of an ASIO buffer pair
typedef struct _ASIO_BUFFER {
    PUCHAR Buffer;
    SIZE_T Size;
} ASIO_BUFFER, *PASIO_BUFFER;

typedef struct _ASIO_BUFFER_CONTEXT {
    ASIO_BUFFER Input[2];
    ASIO_BUFFER Output[2];
    ULONG CurrentIndex;
} ASIO_BUFFER_CONTEXT, *PASIO_BUFFER_CONTEXT;

NTSTATUS InitBuffers(_In_ WDFDEVICE Device, _Out_ PASIO_BUFFER_CONTEXT Context);
VOID     ReleaseBuffers(_Inout_ PASIO_BUFFER_CONTEXT Context);
NTSTATUS ProcessAudioBuffer(_Inout_ PASIO_BUFFER_CONTEXT Context);

// Simple ring buffer for audio streaming
typedef struct _RING_BUFFER {
    PUCHAR        Buffer;
    ULONG         Size;
    volatile ULONG ReadPos;
    volatile ULONG WritePos;
} RING_BUFFER, *PRING_BUFFER;

typedef struct _STREAM_CONTEXT {
    RING_BUFFER InputRing;
    RING_BUFFER OutputRing;
    WDFTIMER    BufferTimer; // Placeholder periodic timer
} STREAM_CONTEXT, *PSTREAM_CONTEXT;

NTSTATUS InitRingBuffers(_In_ WDFDEVICE Device, _Out_ PSTREAM_CONTEXT Context);
VOID     ReleaseRingBuffers(_Inout_ PSTREAM_CONTEXT Context);

EVT_WDF_TIMER BufferTimerFunc;
