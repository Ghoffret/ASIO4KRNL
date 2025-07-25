#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <ks.h>
#include <ksmedia.h>

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
    void     LogUnderrun();

    ULONG GetBufferSize() const { return m_bufferFrames; }
    ULONG GetSampleRate() const { return m_sampleRate; }
    ULONG GetChannelCount() const { return m_channels; }

private:
    NTSTATUS CreatePins();
    NTSTATUS ConfigureBuffer();
    void     LogLatency();

    WDFDEVICE m_device;
    HANDLE    m_pinIn;
    HANDLE    m_pinOut;

    ULONG  m_sampleRate;
    ULONG  m_channels;
    ULONG  m_bufferFrames;
    PUCHAR m_buffer;
    SIZE_T m_bufferSize;
};

