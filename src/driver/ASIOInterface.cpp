#include "Driver.h"
#include <windef.h>

extern "C" {

// These stubs mimic the layout of the Steinberg ASIO interface.
// They will eventually be exported via COM or registry entries so
// that DAWs can discover the driver.

__declspec(dllexport) LONG ASIOInit(void* sysHandle)
{
    UNREFERENCED_PARAMETER(sysHandle);
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: ASIOInit called\n"));
    return 0; // ASE_OK placeholder
}

__declspec(dllexport) LONG ASIOGetBufferSize(LONG* minSize, LONG* maxSize, LONG* preferredSize, LONG* granularity)
{
    UNREFERENCED_PARAMETER(minSize);
    UNREFERENCED_PARAMETER(maxSize);
    UNREFERENCED_PARAMETER(preferredSize);
    UNREFERENCED_PARAMETER(granularity);
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: ASIOGetBufferSize called\n"));
    return 0;
}

__declspec(dllexport) LONG ASIOCreateBuffers(void* bufferInfos, LONG numChannels, LONG bufferSize, void* callbacks)
{
    UNREFERENCED_PARAMETER(bufferInfos);
    UNREFERENCED_PARAMETER(numChannels);
    UNREFERENCED_PARAMETER(bufferSize);
    UNREFERENCED_PARAMETER(callbacks);
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: ASIOCreateBuffers called\n"));
    return 0;
}

__declspec(dllexport) LONG ASIOStart()
{
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: ASIOStart called\n"));
    return 0;
}

__declspec(dllexport) LONG ASIOStop()
{
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "ASIO4KRNL: ASIOStop called\n"));
    return 0;
}

}

