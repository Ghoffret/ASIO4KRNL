#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <ks.h>
#include <ksmedia.h>
#include "Driver.h"

// Core audio engine handling WDM-KS streaming without ASIO4ALL style wrappers.
class KSAudioEngine {
public:
    KSAudioEngine();
    ~KSAudioEngine();

    // Initialize engine with desired sample rate, channel count and buffer size.
    NTSTATUS Initialize(_In_opt_ WDFDEVICE device,
                        _In_ ULONG sampleRate,
                        _In_ ULONG channelCount,
                        _In_ ULONG bufferFrames);

    NTSTATUS Start();
    NTSTATUS Stop();
    void     Shutdown();

    // Logging helpers
    ASIO4KRNL_INLINE void LogUnderrun();

    ASIO4KRNL_INLINE ULONG GetBufferSize() const { return m_bufferFrames; }
    ASIO4KRNL_INLINE ULONG GetSampleRate() const { return m_sampleRate; }
    ASIO4KRNL_INLINE ULONG GetChannelCount() const { return m_channels; }
    ASIO4KRNL_INLINE ULONG GetLatencyMs() const { 
        if (!m_latencyCacheValid && m_sampleRate != 0) {
            m_cachedLatencyMs = (m_bufferFrames * 1000) / m_sampleRate;
            m_latencyCacheValid = true;
        }
        return m_cachedLatencyMs; 
    }

private:
    NTSTATUS CreatePins();
    NTSTATUS ConfigureBuffer();
    ASIO4KRNL_INLINE void LogLatency();

    WDFDEVICE m_device;
    HANDLE    m_pinIn;
    HANDLE    m_pinOut;

    ULONG  m_sampleRate;
    ULONG  m_channels;
    ULONG  m_bufferFrames;
    PUCHAR m_buffer;
    SIZE_T m_bufferSize;
    
    // Optimize: Add cached values for frequently used calculations
    mutable ULONG m_cachedLatencyMs;
    mutable bool m_latencyCacheValid;
};

