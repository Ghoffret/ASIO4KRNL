#include "Driver.h"
#include "KSAudioEngine.h"
#include <windef.h>

extern KSAudioEngine g_KsEngine;

extern "C" {

// These stubs mimic the layout of the Steinberg ASIO interface.
// They will eventually be exported via COM or registry entries so
// that DAWs can discover the driver.

__declspec(dllexport) LONG ASIOInit(void* sysHandle)
{
    UNREFERENCED_PARAMETER(sysHandle);
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: ASIOInit called\n"));
    NTSTATUS status = g_KsEngine.Initialize(nullptr, 48000, 2, 256);
    return NT_SUCCESS(status) ? 0 : -1;
}

__declspec(dllexport) LONG ASIOGetBufferSize(LONG* minSize, LONG* maxSize, LONG* preferredSize, LONG* granularity)
{
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: ASIOGetBufferSize called\n"));
    if (minSize) *minSize = 64;
    if (maxSize) *maxSize = g_KsEngine.GetBufferSize();
    if (preferredSize) *preferredSize = g_KsEngine.GetBufferSize();
    if (granularity) *granularity = 0;
    return 0;
}

__declspec(dllexport) LONG ASIOCreateBuffers(void* bufferInfos, LONG numChannels, LONG bufferSize, void* callbacks)
{
    UNREFERENCED_PARAMETER(bufferInfos);
    UNREFERENCED_PARAMETER(callbacks);
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: ASIOCreateBuffers called ch=%ld size=%ld\n", numChannels, bufferSize));
    g_KsEngine.Shutdown();
    NTSTATUS status = g_KsEngine.Initialize(nullptr, 48000, numChannels, bufferSize);
    return NT_SUCCESS(status) ? 0 : -1;
}

__declspec(dllexport) LONG ASIOStart()
{
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: ASIOStart called\n"));
    return NT_SUCCESS(g_KsEngine.Start()) ? 0 : -1;
}

__declspec(dllexport) LONG ASIOStop()
{
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: ASIOStop called\n"));
    NTSTATUS status = g_KsEngine.Stop();
    g_KsEngine.Shutdown();
    return NT_SUCCESS(status) ? 0 : -1;
}

}

