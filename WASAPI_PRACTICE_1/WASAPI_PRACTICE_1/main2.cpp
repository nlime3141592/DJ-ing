
// Simple example code to load a Wav file and play it with WASAPI
// This is NOT complete Wav loading code. It is a barebones example 
// that makes a lot of assumptions, see the assert() calls for details
//
// References: 
// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
// Handmade Hero Day 138: Loading WAV Files

// 참고자료: https://gist.github.com/kevinmoran/3d05e190fb4e7f27c1043a3ba321cede

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#include <assert.h>
#include <stdint.h>

#include "audiofilter.h"
#include "audioutil.h"
#include "audioreverb.h"

#include "hidapi.h"

#include <stdio.h>

// Struct to get data from loaded WAV file.
// NB: This will only work for WAV files containing PCM (non-compressed) data
// otherwise the layout will be different.
#pragma warning(disable : 4200)
struct WavFile {
    // RIFF Chunk
    uint32_t riffId;
    uint32_t riffChunkSize;
    uint32_t waveId;

    // fmt Chunk
    uint32_t fmtId;
    uint32_t fmtChunkSize;
    uint16_t formatCode;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    // These are not present for PCM Wav Files
    // uint16_t cbSize;
    // uint16_t wValidBitsPerSample;
    // uint32_t dwChannelMask;
    // char subFormatGUID[16];

    // data Chunk
    uint32_t dataId;
    uint32_t dataChunkSize;
    uint16_t samples[]; // actual samples start here
};
#pragma warning(default : 4200)

int pressed1 = 0;
int pressed2 = 0;

bool Win32LoadEntireFile(const char* filename, void** data, uint32_t* numBytesRead)
{
    HANDLE file = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if ((file == INVALID_HANDLE_VALUE)) return false;

    DWORD fileSize = GetFileSize(file, 0);
    if (!fileSize) return false;

    *data = HeapAlloc(GetProcessHeap(), 0, fileSize + 1);
    if (!*data) return false;

    if (!ReadFile(file, *data, fileSize, (LPDWORD)numBytesRead, 0))
        return false;

    CloseHandle(file);
    ((uint8_t*)*data)[fileSize] = 0;

    return true;
}

void Win32FreeFileData(void* data)
{
    HeapFree(GetProcessHeap(), 0, data);
}

int main()
{
    void* fileBytes;
    uint32_t fileSize;
    bool result = Win32LoadEntireFile("C:\\Test\\loop1.wav", &fileBytes, &fileSize);
    assert(result);

    WavFile* wav = (WavFile*)fileBytes;
    // Check the Chunk IDs to make sure we loaded the file correctly
    assert(wav->riffId == 1179011410);
    assert(wav->waveId == 1163280727);
    assert(wav->fmtId == 544501094);
    assert(wav->dataId == 1635017060);
    // Check data is in format we expect
    assert(wav->formatCode == 1); // Only support PCM data
    assert(wav->numChannels == 2); // Only support 2-channel data
    assert(wav->fmtChunkSize == 16); // This should be true for PCM data
    assert(wav->sampleRate == 44100); // Only support 44100Hz data
    assert(wav->bitsPerSample == 16); // Only support 16-bit samples
    // This is how these fields are defined, no harm to assert that they're what we expect
    assert(wav->blockAlign == wav->numChannels * wav->bitsPerSample / 8);
    assert(wav->byteRate == wav->sampleRate * wav->blockAlign);

    uint32_t numWavSamples = wav->dataChunkSize / sizeof(uint16_t);
    uint16_t* wavSamples = wav->samples;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
    assert(hr == S_OK);

    IMMDeviceEnumerator* deviceEnumerator;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (LPVOID*)(&deviceEnumerator));
    assert(hr == S_OK);

    IMMDevice* audioDevice;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice);
    assert(hr == S_OK);

    deviceEnumerator->Release();

    IAudioClient2* audioClient;
    hr = audioDevice->Activate(__uuidof(IAudioClient2), CLSCTX_ALL, nullptr, (LPVOID*)(&audioClient));
    assert(hr == S_OK);

    audioDevice->Release();

    // WAVEFORMATEX* defaultMixFormat = NULL;
    // hr = audioClient->GetMixFormat(&defaultMixFormat);
    // assert(hr == S_OK);

    WAVEFORMATEX mixFormat = {};
    mixFormat.wFormatTag = WAVE_FORMAT_PCM;
    mixFormat.nChannels = 2;
    mixFormat.nSamplesPerSec = 44100;//defaultMixFormat->nSamplesPerSec;
    mixFormat.wBitsPerSample = 16;
    mixFormat.nBlockAlign = (mixFormat.nChannels * mixFormat.wBitsPerSample) / 8;
    mixFormat.nAvgBytesPerSec = mixFormat.nSamplesPerSec * mixFormat.nBlockAlign;

    const float BUFFER_SIZE_IN_SECONDS = 2.0f;
    const int64_t REFTIMES_PER_SEC = 10000000; // hundred nanoseconds
    REFERENCE_TIME requestedSoundBufferDuration = (REFERENCE_TIME)(REFTIMES_PER_SEC * BUFFER_SIZE_IN_SECONDS);
    DWORD initStreamFlags = (AUDCLNT_STREAMFLAGS_RATEADJUST
        | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
        | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);
    hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
        initStreamFlags,
        requestedSoundBufferDuration,
        0, &mixFormat, nullptr);
    assert(hr == S_OK);

    IAudioRenderClient* audioRenderClient;
    hr = audioClient->GetService(__uuidof(IAudioRenderClient), (LPVOID*)(&audioRenderClient));
    assert(hr == S_OK);

    UINT32 bufferSizeInFrames;
    hr = audioClient->GetBufferSize(&bufferSizeInFrames);
    assert(hr == S_OK);

    hr = audioClient->Start();
    assert(hr == S_OK);

    // 필터 검증 로직
#ifdef REGION_FILTER_1
    Biquad biquad_l; // 왼쪽 채널 오디오 필터
    Biquad biquad_r; // 오른쪽 채널 오디오 필터

    float q = INV_SQRT_2;
    float f = 2000.0f;
    float fs = wav->sampleRate;
    float gain = -30.0f;
    InitParamsLPF(&biquad_l, f, fs);
    InitParamsLPF(&biquad_r, f, fs);

    //SetParamsLSF_Safe(&biquad_l, q, f, fs, gain);
    //SetParamsLSF_Safe(&biquad_r, q, f, fs, gain);
    SetParamsLPF_Safe(&biquad_l, q, f, fs);
    SetParamsLPF_Safe(&biquad_r, q, f, fs);
#endif

#define REGION_REVERB_1
#ifdef REGION_REVERB_1
    SchroederReverb lReverb(wav->sampleRate);
    SchroederReverb rReverb(wav->sampleRate);
    float roomSize = 0.8f;
    float damping = 0.1f;
    float wet = 0.2f;
    
    //lReverb.setParams(roomSize, damping, wet, dry);
    //rReverb.setParams(roomSize, damping, wet, dry);
#endif

    // autocorrelation 검증 로직
#ifdef REGION_AUTO_CORRELATION_1
    float bpmMin = 128.0f;
    float bpmMax = 132.0f;
    float bpmDelta = 0.25f;
    int16_t* b;
    int offset = 0;
    int length = numWavSamples;

    printf("waiting for auto-correlation calculation...\n");

    AutoCorrelation autoOutput;

    float bpm = GetAutoCorrelation(
        &autoOutput,
        bpmMin, bpmMax, bpmDelta,
        (int16_t*)wavSamples, offset, length, fs,
        2
    );

    int jumpMadi = 17;
    int shiftMadi = 0;

    printf("selected bpm == %f\n", autoOutput.bpm);
    printf("selected off == %i\n", autoOutput.offset);
    printf("selected tau == %i\n", autoOutput.tau);
#endif

    // 루프 제어
#ifdef REGION_LOOP_1
    int lBegin = -1;
    int lLength = -1;
    int lTau = -1;
#endif

#define REGION_HID_INPUT_1
#ifdef REGION_HID_INPUT_1
    int idVendor = 0x2341;
    int idProduct = 0x8036;

    hid_device* device = hid_open(idVendor, idProduct, NULL);
    
    if (!device)
    {
        printf("Device not found.\n");
        return -1;
    }

    hid_set_nonblocking(device, true);

    uint8_t reportBuffer[9] = { 0 };
#endif

    // 재생을 위한 버퍼링 동작
    int wavPlaybackSample = 0;
    while (true)
    {
        // Padding is how much valid data is queued up in the sound buffer
        // if there's enough padding then we could skip writing more data
        UINT32 bufferPadding;
        hr = audioClient->GetCurrentPadding(&bufferPadding);
        assert(hr == S_OK);

        // How much padding we want our sound buffer to have after writing to it.
        // Needs to be enough so that the playback doesn't reach garbage data
        // but we get less latency the lower it is (i.e. how long does it take
        // between pressing jump and hearing the sound effect)
        // Try setting this to e.g. 1/250.f to hear what happens when
        // we're not writing enough data to stay ahead of playback!
        const float TARGET_BUFFER_PADDING_IN_SECONDS = 1 / 60.f;
        UINT32 targetBufferPadding = UINT32(bufferSizeInFrames * TARGET_BUFFER_PADDING_IN_SECONDS);
        UINT32 numFramesToWrite = targetBufferPadding - bufferPadding;

        int16_t* buffer;
        hr = audioRenderClient->GetBuffer(numFramesToWrite, (BYTE**)(&buffer));
        assert(hr == S_OK);

        for (UINT32 frameIndex = 0; frameIndex < numFramesToWrite; ++frameIndex)
        {
            // 키 입력 감지 시 바로 특정 샘플 위치로 점프하는 기능
            int p1 = GetAsyncKeyState(VK_F8) & 0x8000;
            int p2 = GetAsyncKeyState(VK_F7) & 0x8000;
            
            if (!pressed1 && p1)
            {

            }

            if (!pressed2 && p2)
            {

            }

            pressed1 = p1;
            pressed2 = p2;

            int hidLength = hid_read(device, reportBuffer, sizeof(reportBuffer));

            if (hidLength < 0)
            {
                break;
            }
            else if (hidLength > 0)
            {
                float iRoomSize = (float)reportBuffer[3] / 255.0f;
                float iDamping = (float)reportBuffer[4] / 255.0f;
                float iWet = (float)reportBuffer[5] / 255.0f;

                lReverb.SetRoomSize(iRoomSize);
                lReverb.SetDamping(iDamping);
                lReverb.SetWet(iWet);

                rReverb.SetRoomSize(iRoomSize);
                rReverb.SetDamping(iDamping);
                rReverb.SetWet(iWet);

                printf("RoomSize == %0.03f   Damping == %0.03f   Wet == %0.03f\n", iRoomSize, iDamping, iWet);
            }

            int16_t lSample = wavSamples[wavPlaybackSample];
            wavPlaybackSample = (wavPlaybackSample + 1) % numWavSamples;
            int16_t rSample = wavSamples[wavPlaybackSample];
            wavPlaybackSample = (wavPlaybackSample + 1) % numWavSamples;

            // left
            //*buffer++ = lSample;
            float lReverbValue = static_cast<float>(lSample) / 32768.0f;
            lReverbValue = lReverb.Process(lReverbValue);
            lReverbValue = lReverbValue * 32768.0f;
            lReverbValue = max(-32768.0f, min(lReverbValue, 32767.0f));
            *buffer++ = static_cast<uint16_t>(lReverbValue);
            //*buffer++ = BiquadConvolution(&biquad_l, lSample);

            // right
            //*buffer++ = rSample;
            float rReverbValue = static_cast<float>(rSample) / 32768.0f;
            rReverbValue = rReverb.Process(rReverbValue);
            rReverbValue = rReverbValue * 32768.0f;
            rReverbValue = max(-32768.0f, min(rReverbValue, 32767.0f));
            *buffer++ = static_cast<uint16_t>(rReverbValue);
            //*buffer++ = BiquadConvolution(&biquad_r, rSample);
        }

        hr = audioRenderClient->ReleaseBuffer(numFramesToWrite, 0);
        assert(hr == S_OK);

        // Get playback cursor position
        // This is good for visualising playback and seeing the reading/writing in action!
        IAudioClock* audioClock;
        audioClient->GetService(__uuidof(IAudioClock), (LPVOID*)(&audioClock));
        UINT64 audioPlaybackFreq;
        UINT64 audioPlaybackPos;
        audioClock->GetFrequency(&audioPlaybackFreq);
        audioClock->GetPosition(&audioPlaybackPos, 0);
        audioClock->Release();
        // UINT64 audioPlaybackPosInSeconds = audioPlaybackPos/audioPlaybackFreq;
        // UINT64 audioPlaybackPosInSamples = audioPlaybackPosInSeconds*mixFormat.nSamplesPerSec;
    }

    audioClient->Stop();
    audioClient->Release();
    audioRenderClient->Release();

    Win32FreeFileData(fileBytes);

    return 0;
}