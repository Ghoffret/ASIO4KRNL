#include "KSAudioEngine.h"

#define TAG_KSENG 'GSKA'

KSAudioEngine::KSAudioEngine()
    : m_device(WDF_NO_HANDLE),
      m_pinIn(nullptr),
      m_pinOut(nullptr),
      m_sampleRate(0),
      m_channels(0),
      m_bufferFrames(0),
      m_buffer(nullptr),
      m_bufferSize(0) {}

KSAudioEngine::~KSAudioEngine() {
    Shutdown();
}

NTSTATUS KSAudioEngine::Initialize(_In_opt_ WDFDEVICE device,
                                   _In_ ULONG sampleRate,
                                   _In_ ULONG channelCount,
                                   _In_ ULONG bufferFrames) {
    m_device = device;
    m_sampleRate = sampleRate;
    m_channels = channelCount;
    m_bufferFrames = bufferFrames;

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
               "ASIO4KRNL: Initializing KS engine SR=%lu ch=%lu frames=%lu\n",
               sampleRate, channelCount, bufferFrames));

    NTSTATUS status = CreatePins();
    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = ConfigureBuffer();
    if (NT_SUCCESS(status)) {
        LogLatency();
    }
    return status;
}

NTSTATUS KSAudioEngine::CreatePins() {
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
               "ASIO4KRNL: Creating WDM-KS pins (stub)\n"));
    // TODO: create capture and render pins using KsCreatePin
    m_pinIn = nullptr;
    m_pinOut = nullptr;
    return STATUS_SUCCESS;
}

NTSTATUS KSAudioEngine::ConfigureBuffer() {
    m_bufferSize = m_bufferFrames * m_channels * sizeof(USHORT);
    m_buffer = static_cast<PUCHAR>(ExAllocatePoolWithTag(NonPagedPoolNx,
                                                        m_bufferSize,
                                                        TAG_KSENG));
    if (!m_buffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    RtlZeroMemory(m_buffer, m_bufferSize);
    return STATUS_SUCCESS;
}

void KSAudioEngine::LogLatency() {
    ULONG latencyMs = 0;
    if (m_sampleRate) {
        latencyMs = (m_bufferFrames * 1000) / m_sampleRate;
    }
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
               "ASIO4KRNL: Estimated latency %lu ms\n", latencyMs));
}

void KSAudioEngine::LogUnderrun() {
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
               "ASIO4KRNL: Buffer underrun detected\n"));
}

NTSTATUS KSAudioEngine::Start() {
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
               "ASIO4KRNL: Starting KS streaming (stub)\n"));
    // TODO: transition pins to KSSTATE_RUN and begin transfer
    return STATUS_SUCCESS;
}

NTSTATUS KSAudioEngine::Stop() {
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
               "ASIO4KRNL: Stopping KS streaming (stub)\n"));
    // TODO: transition pins to KSSTATE_STOP
    return STATUS_SUCCESS;
}

void KSAudioEngine::Shutdown() {
    Stop();
    if (m_pinIn) {
        ZwClose(m_pinIn);
        m_pinIn = nullptr;
    }
    if (m_pinOut) {
        ZwClose(m_pinOut);
        m_pinOut = nullptr;
    }
    if (m_buffer) {
        ExFreePoolWithTag(m_buffer, TAG_KSENG);
        m_buffer = nullptr;
        m_bufferSize = 0;
    }
}

