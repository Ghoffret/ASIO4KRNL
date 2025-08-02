#include "KSAudioEngine.h"
#include "Driver.h"

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

    ASIO4KRNL_LOG_INFO("Initializing KS engine SR=%lu ch=%lu frames=%lu\n",
                       sampleRate, channelCount, bufferFrames);

    NTSTATUS status = CreatePins();
    if (ASIO4KRNL_UNLIKELY(!NT_SUCCESS(status))) {
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
    // Optimize: Calculate buffer size based on actual audio format requirements
    // 16-bit samples (2 bytes) * channels * frames for optimal memory usage
    m_bufferSize = m_bufferFrames * m_channels * sizeof(USHORT);
    
    m_buffer = static_cast<PUCHAR>(ExAllocatePoolWithTag(NonPagedPoolNx,
                                                        m_bufferSize,
                                                        TAG_KSENG));
    if (!m_buffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    // Optimize: Use RtlZeroMemory for better performance than individual initialization
    RtlZeroMemory(m_buffer, m_bufferSize);
    return STATUS_SUCCESS;
}

ASIO4KRNL_INLINE void KSAudioEngine::LogLatency() {
    // Optimize: Avoid division if sample rate is zero (fast path)
    if (ASIO4KRNL_LIKELY(m_sampleRate != 0)) {
        const ULONG latencyMs = (m_bufferFrames * 1000) / m_sampleRate;
        ASIO4KRNL_LOG_INFO("Estimated latency %lu ms\n", latencyMs);
    }
}

ASIO4KRNL_INLINE void KSAudioEngine::LogUnderrun() {
    ASIO4KRNL_LOG_ERROR("Buffer underrun detected\n");
}

NTSTATUS KSAudioEngine::Start() {
    ASIO4KRNL_LOG_INFO("Starting KS streaming (stub)\n");
    // TODO: transition pins to KSSTATE_RUN and begin transfer
    return STATUS_SUCCESS;
}

NTSTATUS KSAudioEngine::Stop() {
    ASIO4KRNL_LOG_INFO("Stopping KS streaming (stub)\n");
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

