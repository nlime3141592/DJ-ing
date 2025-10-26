
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
#include "pcmwav.h"
#include "audiochannel.h"
#include "audiodevice.h"

#include <stdio.h>

int main()
{
    // 출력 디바이스 초기화 시작
    InitAudioDevice_Global();

    AudioDevice device = AudioDevice();

    device.Init();

    // TODO: 왜 있는 코드인지 알아내기
    // WAVEFORMATEX* defaultMixFormat = NULL;
    // hr = audioClient->GetMixFormat(&defaultMixFormat);
    // assert(hr == S_OK);

    device.Start();

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

    // 리버브 검증 로직
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

    hid_device* hidDevice = hid_open(idVendor, idProduct, NULL);
    
    if (!hidDevice)
    {
        printf("Device not found.\n");
        return -1;
    }

    hid_set_nonblocking(hidDevice, true);

    uint8_t reportBuffer[9] = { 0 };
#endif

#define REGION_AUDIO_CHANNEL_1
#ifdef REGION_AUDIO_CHANNEL_1
    AudioChannel c0 = AudioChannel();

    bool result = c0.Load("C:\\Test\\habibi.wav");
    assert(result);

    c0.Play();
#endif

    int pD6 = 0;
    int pD7 = 0;
    int pD8 = 0;
    int pD9 = 0;

    UINT32 bufferSizeInFrames;
    HRESULT hr = device.audioClient->GetBufferSize(&bufferSizeInFrames);
    assert(hr == S_OK);

    while (true)
    {
        // Padding is how much valid data is queued up in the sound buffer
        // if there's enough padding then we could skip writing more data
        UINT32 bufferPadding;
        hr = device.audioClient->GetCurrentPadding(&bufferPadding);
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
        hr = device.audioRenderClient->GetBuffer(numFramesToWrite, (BYTE**)(&buffer));
        assert(hr == S_OK);

        for (UINT32 frameIndex = 0; frameIndex < numFramesToWrite; ++frameIndex)
        {
            int hidLength = hid_read(hidDevice, reportBuffer, sizeof(reportBuffer));

            if (hidLength < 0)
            {
                break;
            }
            else if (hidLength > 0)
            {
                switch (reportBuffer[2])
                {
                case 0: // digital data
                    if (pD6 == 0 && reportBuffer[3] != 0)
                    {
                        printf("Down D6\n");
                        pD6 = reportBuffer[3];

                        // down
                        if (c0.isPlaying)
                            c0.Pause();
                        else
                            c0.Play();
                    }
                    if (pD6 != 0 && reportBuffer[3] == 0)
                    {
                        printf("Up D6\n");
                        pD6 = 0;

                        // up
                    }
                    if (pD7 == 0 && reportBuffer[4] != 0)
                    {
                        printf("Down D7\n");
                        pD7 = reportBuffer[4];

                        // down
                        c0.JumpImmediate((int32_t)(64.75f * 88200.0f));
                    }
                    if (pD7 != 0 && reportBuffer[4] == 0)
                    {
                        printf("Up D7\n");
                        pD7 = 0;

                        // up
                    }
                    if (pD8 == 0 && reportBuffer[5] != 0)
                    {
                        printf("Down D8\n");
                        pD8 = reportBuffer[5];

                        // down
                    }
                    if (pD8 != 0 && reportBuffer[5] == 0)
                    {
                        printf("Up D8\n");
                        pD8 = 0;

                        // up
                    }
                    if (pD9 == 0 && reportBuffer[6] != 0)
                    {
                        printf("Down D9\n");
                        pD9 = reportBuffer[6];

                        // down
                    }
                    if (pD9 != 0 && reportBuffer[6] == 0)
                    {
                        printf("Up D9\n");
                        pD9 = 0;

                        // up
                    }
                    break;
                case 1: // analog data
                    c0.masterVolume = (float)reportBuffer[3] / 255.0f;
                    break;
                default:
                    break;
                }
            }

            int16_t lSample;
            int16_t rSample;

            if (!c0.Read(&lSample, &rSample))
            {
                //printf("Read Failed\n");
            }
            else
            {
                //printf("%06d, %06d\n", lSample, rSample);
            }

            // left
            *buffer++ = lSample;
            //float lReverbValue = static_cast<float>(lSample) / 32768.0f;
            //lReverbValue = lReverb.Process(lReverbValue);
            //lReverbValue = lReverbValue * 32768.0f;
            //lReverbValue = max(-32768.0f, min(lReverbValue, 32767.0f));
            //*buffer++ = static_cast<uint16_t>(lReverbValue);
            //*buffer++ = BiquadConvolution(&biquad_l, lSample);

            // right
            *buffer++ = rSample;
            //float rReverbValue = static_cast<float>(rSample) / 32768.0f;
            //rReverbValue = rReverb.Process(rReverbValue);
            //rReverbValue = rReverbValue * 32768.0f;
            //rReverbValue = max(-32768.0f, min(rReverbValue, 32767.0f));
            //*buffer++ = static_cast<uint16_t>(rReverbValue);
            //*buffer++ = BiquadConvolution(&biquad_r, rSample);
        }

        hr = device.audioRenderClient->ReleaseBuffer(numFramesToWrite, 0);
        assert(hr == S_OK);

        // Get playback cursor position
        // This is good for visualising playback and seeing the reading/writing in action!
        IAudioClock* audioClock;
        device.audioClient->GetService(__uuidof(IAudioClock), (LPVOID*)(&audioClock));
        UINT64 audioPlaybackFreq;
        UINT64 audioPlaybackPos;
        audioClock->GetFrequency(&audioPlaybackFreq);
        audioClock->GetPosition(&audioPlaybackPos, 0);
        audioClock->Release();
        // UINT64 audioPlaybackPosInSeconds = audioPlaybackPos/audioPlaybackFreq;
        // UINT64 audioPlaybackPosInSamples = audioPlaybackPosInSeconds*mixFormat.nSamplesPerSec;
    }

    device.audioClient->Stop();
    device.audioClient->Release();
    device.audioRenderClient->Release();

    c0.Unload();
    //Win32FreeFileData(fileBytes);

    return 0;
}