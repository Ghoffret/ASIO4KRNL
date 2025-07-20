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

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: Enumerating USB audio interfaces\n"));

    WDF_USB_DEVICE_CREATE_CONFIG config;
    WDF_USB_DEVICE_CREATE_CONFIG_INIT(&config, USBD_CLIENT_CONTRACT_VERSION_602);

    status = WdfUsbTargetDeviceCreateWithParameters(Device,
                                                    &config,
                                                    WDF_NO_OBJECT_ATTRIBUTES,
                                                    &usbCtx->UsbDevice);
    if (!NT_SUCCESS(status)) {
        KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ASIO4KRNL: failed to create USB device %x\n", status));
        LogMessage("failed to create USB device %x\n", status);
        return status;
    }

    usbCtx->AudioInterface = WdfUsbTargetDeviceGetInterface(usbCtx->UsbDevice, 0);
    if (usbCtx->AudioInterface == NULL) {
        KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ASIO4KRNL: no audio interface found\n"));
        LogMessage("no audio interface found\n");
        return STATUS_UNSUCCESSFUL;
    }

    if (WdfUsbInterfaceGetNumConfiguredPipes(usbCtx->AudioInterface) < 2) {
        KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ASIO4KRNL: insufficient endpoints\n"));
        LogMessage("insufficient USB endpoints\n");
        return STATUS_INVALID_DEVICE_STATE;
    }

    usbCtx->StreamInPipe = WdfUsbInterfaceGetConfiguredPipe(usbCtx->AudioInterface,
                                                             0,
                                                             NULL);
    usbCtx->StreamOutPipe = WdfUsbInterfaceGetConfiguredPipe(usbCtx->AudioInterface,
                                                             1,
                                                             NULL);
    if (!usbCtx->StreamInPipe || !usbCtx->StreamOutPipe) {
        KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ASIO4KRNL: missing stream pipes\n"));
        LogMessage("missing stream pipes\n");
        return STATUS_INVALID_DEVICE_STATE;
    }

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: USB interfaces ready\n"));

    return STATUS_SUCCESS;
}

NTSTATUS NegotiateStreamFormat(_In_ WDFDEVICE Device)
{
    UNREFERENCED_PARAMETER(Device);

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: Negotiating stream format\n"));

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

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: Setting up ASIO buffers\n"));

    status = InitBuffers(Device, &bufferCtx);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = InitRingBuffers(Device, &streamCtx);
    if (!NT_SUCCESS(status)) {
        ReleaseBuffers(&bufferCtx);
        return status;
    }

    // Initialize kernel streaming engine with default parameters
    status = g_KsEngine.Initialize(Device, 48000, 2, 256);
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
    UNREFERENCED_PARAMETER(Device);
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: Synchronizing sample clock\n"));
    // TODO: synchronize sample clock with hardware
    // Placeholder that simply fails gracefully if clock sync not available
    if (Device == NULL) {
        return STATUS_DEVICE_NOT_READY;
    }
    return STATUS_SUCCESS;
}

NTSTATUS InitBuffers(_In_ WDFDEVICE Device, _Out_ PASIO_BUFFER_CONTEXT Context)
{
    UNREFERENCED_PARAMETER(Device);
    RtlZeroMemory(Context, sizeof(ASIO_BUFFER_CONTEXT));
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: Initializing ASIO double buffers\n"));

    ULONG size = 4096; // placeholder buffer size
    for (int i = 0; i < 2; ++i) {
        Context->Input[i].Size = size;
        Context->Input[i].Buffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPoolNx, size, '4ksA');
        if (!Context->Input[i].Buffer) {
            LogMessage("input buffer allocation failed\n");
            ReleaseBuffers(Context);
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        Context->Output[i].Size = size;
        Context->Output[i].Buffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPoolNx, size, '4ksA');
        if (!Context->Output[i].Buffer) {
            LogMessage("output buffer allocation failed\n");
            ReleaseBuffers(Context);
            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    return STATUS_SUCCESS;
}

VOID ReleaseBuffers(_Inout_ PASIO_BUFFER_CONTEXT Context)
{
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: Releasing ASIO buffers\n"));
    for (int i = 0; i < 2; ++i) {
        if (Context->Input[i].Buffer) {
            ExFreePoolWithTag(Context->Input[i].Buffer, '4ksA');
            Context->Input[i].Buffer = NULL;
        }
        if (Context->Output[i].Buffer) {
            ExFreePoolWithTag(Context->Output[i].Buffer, '4ksA');
            Context->Output[i].Buffer = NULL;
        }
    }
}

NTSTATUS ProcessAudioBuffer(_Inout_ PASIO_BUFFER_CONTEXT Context)
{
    if (!Context)
        return STATUS_INVALID_PARAMETER;

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
               "ASIO4KRNL: Processing ASIO buffer index %lu\n",
               Context->CurrentIndex));

    // Simple ping‑pong of the active buffer index
    Context->CurrentIndex ^= 1;
    return STATUS_SUCCESS;
}

VOID
BufferTimerFunc(
    _In_ WDFTIMER Timer
    )
{
    UNREFERENCED_PARAMETER(Timer);
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: Buffer timer fired\n"));
    // TODO: move data between ring buffer and USB pipes here
    g_KsEngine.LogUnderrun();
}

NTSTATUS InitRingBuffers(_In_ WDFDEVICE Device, _Out_ PSTREAM_CONTEXT Context)
{
    UNREFERENCED_PARAMETER(Device);
    RtlZeroMemory(Context, sizeof(STREAM_CONTEXT));

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: Initializing ring buffers\n"));

    // Allocate simple ring buffers for beta testing
    ULONG rbSize = 16384; // 16kB per ring
    Context->InputRing.Buffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPoolNx,
                                                              rbSize,
                                                              '4ksR');
    Context->OutputRing.Buffer = (PUCHAR)ExAllocatePoolWithTag(NonPagedPoolNx,
                                                               rbSize,
                                                               '4ksR');
    if (!Context->InputRing.Buffer || !Context->OutputRing.Buffer) {
        if (Context->InputRing.Buffer) {
            ExFreePoolWithTag(Context->InputRing.Buffer, '4ksR');
            Context->InputRing.Buffer = NULL;
        }
        if (Context->OutputRing.Buffer) {
            ExFreePoolWithTag(Context->OutputRing.Buffer, '4ksR');
            Context->OutputRing.Buffer = NULL;
        }
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    Context->InputRing.Size  = rbSize;
    Context->OutputRing.Size = rbSize;
    Context->InputRing.ReadPos = Context->InputRing.WritePos = 0;
    Context->OutputRing.ReadPos = Context->OutputRing.WritePos = 0;

    WDF_TIMER_CONFIG timerCfg;
    WDF_OBJECT_ATTRIBUTES attrs;
    WDF_TIMER_CONFIG_INIT_PERIODIC(&timerCfg, BufferTimerFunc, 1); // 1ms placeholder
    WDF_OBJECT_ATTRIBUTES_INIT(&attrs);
    attrs.ParentObject = Device;
    NTSTATUS status = WdfTimerCreate(&timerCfg, &attrs, &Context->BufferTimer);
    if (!NT_SUCCESS(status)) {
        KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ASIO4KRNL: timer create failed %x\n", status));
        LogMessage("timer creation failed %x\n", status);
    }
    return status;
}

VOID ReleaseRingBuffers(_Inout_ PSTREAM_CONTEXT Context)
{
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: Releasing ring buffers\n"));
    if (Context->BufferTimer) {
        WdfTimerStop(Context->BufferTimer, TRUE);
        WdfObjectDelete(Context->BufferTimer);
        Context->BufferTimer = NULL;
    }

    if (Context->InputRing.Buffer) {
        ExFreePoolWithTag(Context->InputRing.Buffer, '4ksR');
        Context->InputRing.Buffer = NULL;
    }
    if (Context->OutputRing.Buffer) {
        ExFreePoolWithTag(Context->OutputRing.Buffer, '4ksR');
        Context->OutputRing.Buffer = NULL;
    }
}
