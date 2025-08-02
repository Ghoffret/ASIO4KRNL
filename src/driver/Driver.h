#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <wdfusb.h>

// Performance and buffer size constants (power of 2 for efficient operations)
#define ASIO_BUFFER_SIZE_BYTES      4096   // 4KB
#define RING_BUFFER_SIZE_BYTES      16384  // 16KB (power of 2)
#define RING_BUFFER_SIZE_MASK       (RING_BUFFER_SIZE_BYTES - 1)  // For efficient modulo
#define BUFFER_TIMER_PERIOD_MS      1
#define DEFAULT_SAMPLE_RATE         48000
#define DEFAULT_CHANNEL_COUNT       2
#define DEFAULT_BUFFER_FRAMES       256

// Pool tags for memory allocation tracking
#define TAG_ASIO_BUFFER    '4ksA'
#define TAG_RING_BUFFER    '4ksR'

// Performance optimization macros
#define ASIO4KRNL_INLINE __forceinline
#define ASIO4KRNL_LIKELY(x) (x)
#define ASIO4KRNL_UNLIKELY(x) (x)

// Optimized logging macros
#define ASIO4KRNL_LOG_INFO(fmt, ...) \
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: " fmt, __VA_ARGS__))
#define ASIO4KRNL_LOG_ERROR(fmt, ...) \
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ASIO4KRNL: " fmt, __VA_ARGS__))

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
ASIO4KRNL_INLINE NTSTATUS ProcessAudioBuffer(_Inout_ PASIO_BUFFER_CONTEXT Context);

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

// Optimized ring buffer operations
ASIO4KRNL_INLINE ULONG RingBufferAvailableRead(_In_ PRING_BUFFER Ring);
ASIO4KRNL_INLINE ULONG RingBufferAvailableWrite(_In_ PRING_BUFFER Ring);
ASIO4KRNL_INLINE VOID RingBufferAdvanceWrite(_Inout_ PRING_BUFFER Ring, _In_ ULONG bytes);
ASIO4KRNL_INLINE VOID RingBufferAdvanceRead(_Inout_ PRING_BUFFER Ring, _In_ ULONG bytes);

EVT_WDF_TIMER BufferTimerFunc;
