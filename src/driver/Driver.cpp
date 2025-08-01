#include "Driver.h"
#include "KSAudioEngine.h"
#include "Log.h"

// Global instance of the kernel streaming engine
KSAudioEngine g_KsEngine;

extern "C" NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;

    WDF_DRIVER_CONFIG_INIT(&config, ASIOEvtDeviceAdd);
    config.EvtDriverUnload = ASIOEvtDriverUnload;

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &config,
                             WDF_NO_HANDLE);
    return status;
}

NTSTATUS
ASIOEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
{
    UNREFERENCED_PARAMETER(Driver);

    WDFDEVICE device;
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, WDFDEVICE);

    status = WdfDeviceCreate(&DeviceInit, &attributes, &device);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = EnumerateUsbInterfaces(device);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = NegotiateStreamFormat(device);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = SetupAsioBuffers(device);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = SyncSampleClock(device);
    return status;
}

VOID
ASIOEvtDriverUnload(
    _In_ WDFDRIVER Driver
    )
{
    UNREFERENCED_PARAMETER(Driver);
    // Cleanup resources here
}

NTSTATUS
ASIOEvtDevicePrepareHardware(
    _In_ WDFDEVICE     Device,
    _In_ WDFCMRESLIST  ResourcesRaw,
    _In_ WDFCMRESLIST  ResourcesTranslated
    )
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(ResourcesRaw);
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    return STATUS_SUCCESS;
}

NTSTATUS
ASIOEvtDeviceReleaseHardware(
    _In_ WDFDEVICE     Device,
    _In_ WDFCMRESLIST  ResourcesTranslated
    )
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    return STATUS_SUCCESS;
}

// Placeholder implementations
NTSTATUS EnumerateUsbInterfaces(_In_ WDFDEVICE Device)
{
    PUSB_DEVICE_CONTEXT usbCtx = UsbGetDeviceContext(Device);
    NTSTATUS status;

    ASIO4KRNL_LOG_INFO("Enumerating USB audio interfaces\n");

    WDF_USB_DEVICE_CREATE_CONFIG config;
    WDF_USB_DEVICE_CREATE_CONFIG_INIT(&config, USBD_CLIENT_CONTRACT_VERSION_602);

    status = WdfUsbTargetDeviceCreateWithParameters(Device,
                                                    &config,
                                                    WDF_NO_OBJECT_ATTRIBUTES,
                                                    &usbCtx->UsbDevice);
    if (ASIO4KRNL_UNLIKELY(!NT_SUCCESS(status))) {
        ASIO4KRNL_LOG_ERROR("failed to create USB device %x\n", status);
        LogMessage("failed to create USB device %x\n", status);
        return status;
    }

    usbCtx->AudioInterface = WdfUsbTargetDeviceGetInterface(usbCtx->UsbDevice, 0);
    if (ASIO4KRNL_UNLIKELY(usbCtx->AudioInterface == NULL)) {
        ASIO4KRNL_LOG_ERROR("no audio interface found\n");
        LogMessage("no audio interface found\n");
        return STATUS_UNSUCCESSFUL;
    }

    if (ASIO4KRNL_UNLIKELY(WdfUsbInterfaceGetNumConfiguredPipes(usbCtx->AudioInterface) < 2)) {
        ASIO4KRNL_LOG_ERROR("insufficient endpoints\n");
        LogMessage("insufficient USB endpoints\n");
        return STATUS_INVALID_DEVICE_STATE;
    }

    usbCtx->StreamInPipe = WdfUsbInterfaceGetConfiguredPipe(usbCtx->AudioInterface, 0, NULL);
    usbCtx->StreamOutPipe = WdfUsbInterfaceGetConfiguredPipe(usbCtx->AudioInterface, 1, NULL);
    if (ASIO4KRNL_UNLIKELY(!usbCtx->StreamInPipe || !usbCtx->StreamOutPipe)) {
        ASIO4KRNL_LOG_ERROR("missing stream pipes\n");
        LogMessage("missing stream pipes\n");
        return STATUS_INVALID_DEVICE_STATE;
    }

    ASIO4KRNL_LOG_INFO("USB interfaces ready\n");
    return STATUS_SUCCESS;
}

NTSTATUS NegotiateStreamFormat(_In_ WDFDEVICE Device)
{
    UNREFERENCED_PARAMETER(Device);

    ASIO4KRNL_LOG_INFO("Negotiating stream format\n");

    // Placeholder for actual format negotiation with the hardware
    // In a real driver this would query supported sample rates and bit depths
    // from the USB descriptors and select the closest match requested by the
    // host application.
    // Default to 48000 Hz, 24-bit for now.

    return STATUS_SUCCESS;
}

NTSTATUS SetupAsioBuffers(_In_ WDFDEVICE Device)
{
    ASIO_BUFFER_CONTEXT bufferCtx = {0};
    STREAM_CONTEXT streamCtx = {0};
    NTSTATUS status;

    ASIO4KRNL_LOG_INFO("Setting up ASIO buffers\n");

    status = InitBuffers(Device, &bufferCtx);
    if (ASIO4KRNL_UNLIKELY(!NT_SUCCESS(status))) {
        return status;
    }

    status = InitRingBuffers(Device, &streamCtx);
    if (ASIO4KRNL_UNLIKELY(!NT_SUCCESS(status))) {
        ReleaseBuffers(&bufferCtx);
        return status;
    }

    // Initialize kernel streaming engine with optimized default parameters
    status = g_KsEngine.Initialize(Device, DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_COUNT, DEFAULT_BUFFER_FRAMES);
    if (NT_SUCCESS(status)) {
        status = g_KsEngine.Start();
    }

    status = ProcessAudioBuffer(&bufferCtx);

    g_KsEngine.Stop();
    g_KsEngine.Shutdown();

    ReleaseRingBuffers(&streamCtx);
    ReleaseBuffers(&bufferCtx);

    return status;
}

NTSTATUS SyncSampleClock(_In_ WDFDEVICE Device)
{
    ASIO4KRNL_LOG_INFO("Synchronizing sample clock\n");
    // TODO: synchronize sample clock with hardware
    // Placeholder that simply fails gracefully if clock sync not available
    if (ASIO4KRNL_UNLIKELY(Device == NULL)) {
        return STATUS_DEVICE_NOT_READY;
    }
    return STATUS_SUCCESS;
}

NTSTATUS InitBuffers(_In_ WDFDEVICE Device, _Out_ PASIO_BUFFER_CONTEXT Context)
{
    UNREFERENCED_PARAMETER(Device);
    RtlZeroMemory(Context, sizeof(ASIO_BUFFER_CONTEXT));
    ASIO4KRNL_LOG_INFO("Initializing ASIO double buffers\n");

    // Optimize: Allocate all buffers in one operation to reduce overhead
    const ULONG totalSize = ASIO_BUFFER_SIZE_BYTES * 4; // 2 input + 2 output buffers
    PUCHAR allBuffers = (PUCHAR)ExAllocatePoolWithTag(NonPagedPoolNx, totalSize, TAG_ASIO_BUFFER);
    if (ASIO4KRNL_UNLIKELY(!allBuffers)) {
        LogMessage("buffer allocation failed, size=%lu\n", totalSize);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // Initialize buffer pointers with calculated offsets
    for (int i = 0; i < 2; ++i) {
        Context->Input[i].Size = ASIO_BUFFER_SIZE_BYTES;
        Context->Input[i].Buffer = allBuffers + (i * ASIO_BUFFER_SIZE_BYTES);
        
        Context->Output[i].Size = ASIO_BUFFER_SIZE_BYTES;
        Context->Output[i].Buffer = allBuffers + ((i + 2) * ASIO_BUFFER_SIZE_BYTES);
    }

    // Zero-initialize all buffers at once
    RtlZeroMemory(allBuffers, totalSize);

    return STATUS_SUCCESS;
}

VOID ReleaseBuffers(_Inout_ PASIO_BUFFER_CONTEXT Context)
{
    ASIO4KRNL_LOG_INFO("Releasing ASIO buffers\n");
    
    // Optimize: Since all buffers were allocated in one block, free only the first buffer pointer
    if (Context->Input[0].Buffer) {
        ExFreePoolWithTag(Context->Input[0].Buffer, TAG_ASIO_BUFFER);
        // Clear all buffer pointers since they were part of the same allocation
        for (int i = 0; i < 2; ++i) {
            Context->Input[i].Buffer = NULL;
            Context->Output[i].Buffer = NULL;
        }
    }
}

ASIO4KRNL_INLINE NTSTATUS ProcessAudioBuffer(_Inout_ PASIO_BUFFER_CONTEXT Context)
{
    // Optimize: Fast parameter validation using likely/unlikely hints
    if (ASIO4KRNL_UNLIKELY(!Context)) {
        return STATUS_INVALID_PARAMETER;
    }

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
               "ASIO4KRNL: Processing ASIO buffer index %lu\n",
               Context->CurrentIndex));

    // Optimize: Simple ping‑pong using XOR for efficient buffer index switch
    Context->CurrentIndex ^= 1;
    return STATUS_SUCCESS;
}

VOID
BufferTimerFunc(
    _In_ WDFTIMER Timer
    )
{
    UNREFERENCED_PARAMETER(Timer);
    ASIO4KRNL_LOG_INFO("Buffer timer fired\n");
    // TODO: move data between ring buffer and USB pipes here
    g_KsEngine.LogUnderrun();
}

NTSTATUS InitRingBuffers(_In_ WDFDEVICE Device, _Out_ PSTREAM_CONTEXT Context)
{
    RtlZeroMemory(Context, sizeof(STREAM_CONTEXT));

    ASIO4KRNL_LOG_INFO("Initializing ring buffers\n");

    // Optimize: Allocate both ring buffers in one operation
    const ULONG totalRingSize = RING_BUFFER_SIZE_BYTES * 2;
    PUCHAR ringBuffers = (PUCHAR)ExAllocatePoolWithTag(NonPagedPoolNx, totalRingSize, TAG_RING_BUFFER);
    if (ASIO4KRNL_UNLIKELY(!ringBuffers)) {
        LogMessage("ring buffer allocation failed, size=%lu\n", totalRingSize);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // Initialize ring buffer structures with calculated offsets
    Context->InputRing.Buffer = ringBuffers;
    Context->InputRing.Size = RING_BUFFER_SIZE_BYTES;
    Context->InputRing.ReadPos = Context->InputRing.WritePos = 0;

    Context->OutputRing.Buffer = ringBuffers + RING_BUFFER_SIZE_BYTES;
    Context->OutputRing.Size = RING_BUFFER_SIZE_BYTES;
    Context->OutputRing.ReadPos = Context->OutputRing.WritePos = 0;

    // Zero-initialize all ring buffers at once
    RtlZeroMemory(ringBuffers, totalRingSize);

    // Create timer with configurable period
    WDF_TIMER_CONFIG timerCfg;
    WDF_OBJECT_ATTRIBUTES attrs;
    WDF_TIMER_CONFIG_INIT_PERIODIC(&timerCfg, BufferTimerFunc, BUFFER_TIMER_PERIOD_MS);
    WDF_OBJECT_ATTRIBUTES_INIT(&attrs);
    attrs.ParentObject = Device;
    
    NTSTATUS status = WdfTimerCreate(&timerCfg, &attrs, &Context->BufferTimer);
    if (ASIO4KRNL_UNLIKELY(!NT_SUCCESS(status))) {
        ExFreePoolWithTag(ringBuffers, TAG_RING_BUFFER);
        Context->InputRing.Buffer = Context->OutputRing.Buffer = NULL;
        ASIO4KRNL_LOG_ERROR("timer create failed %x\n", status);
        LogMessage("timer creation failed %x\n", status);
    }
    return status;
}

VOID ReleaseRingBuffers(_Inout_ PSTREAM_CONTEXT Context)
{
    ASIO4KRNL_LOG_INFO("Releasing ring buffers\n");
    
    if (Context->BufferTimer) {
        WdfTimerStop(Context->BufferTimer, TRUE);
        WdfObjectDelete(Context->BufferTimer);
        Context->BufferTimer = NULL;
    }

    // Optimize: Since both ring buffers were allocated in one block, free only the first buffer pointer
    if (Context->InputRing.Buffer) {
        ExFreePoolWithTag(Context->InputRing.Buffer, TAG_RING_BUFFER);
        Context->InputRing.Buffer = NULL;
        Context->OutputRing.Buffer = NULL;
    }
}

// Optimized ring buffer operations
ASIO4KRNL_INLINE ULONG RingBufferAvailableRead(_In_ PRING_BUFFER Ring)
{
    // Optimize: Use efficient bitwise arithmetic for power-of-2 ring buffer sizes
    const ULONG writePos = Ring->WritePos;
    const ULONG readPos = Ring->ReadPos;
    return (writePos - readPos) & RING_BUFFER_SIZE_MASK;
}

ASIO4KRNL_INLINE ULONG RingBufferAvailableWrite(_In_ PRING_BUFFER Ring)
{
    // Optimize: Keep one byte free to distinguish full from empty
    return Ring->Size - RingBufferAvailableRead(Ring) - 1;
}

ASIO4KRNL_INLINE VOID RingBufferAdvanceWrite(_Inout_ PRING_BUFFER Ring, _In_ ULONG bytes)
{
    // Optimize: Use bitwise AND for power-of-2 ring buffer sizes (much faster than modulo)
    Ring->WritePos = (Ring->WritePos + bytes) & RING_BUFFER_SIZE_MASK;
}

ASIO4KRNL_INLINE VOID RingBufferAdvanceRead(_Inout_ PRING_BUFFER Ring, _In_ ULONG bytes)
{
    // Optimize: Use bitwise AND for power-of-2 ring buffer sizes (much faster than modulo)
    Ring->ReadPos = (Ring->ReadPos + bytes) & RING_BUFFER_SIZE_MASK;
}
