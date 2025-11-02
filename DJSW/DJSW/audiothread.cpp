#include "audiothread.h"

#include "audiodevice.h"
#include "audiochannel.h"

#include "HIDThread.h"

DWORD WINAPI AudioMain(LPVOID lpParam)
{
	//HRESULT hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	assert(hr == S_OK);

	CoInitializeSecurity(nullptr, -1, nullptr, nullptr,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr, EOAC_NONE, nullptr);

	OutputDebugStringW(L"새 오디오 스레드가 시작되었습니다.\n");

	AudioParams* params = (AudioParams*)lpParam;

	AudioDevice device = AudioDevice();

	device.Init();
	device.Start();

	AudioChannel c0 = AudioChannel();

	bool result = c0.Load("C:\\Test\\habibi.wav");
	assert(result);

	c0.Play();

	UINT32 bufferSizeInFrames;
	hr = device.audioClient->GetBufferSize(&bufferSizeInFrames);
	assert(hr == S_OK);

	while (!params->intrHaltThread)
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
			// input here.
			if (GetKeyDown(0x11))
			{
				if (c0.isPlaying)
				{
					c0.Pause();
					OutputDebugStringW(L"Pause\n");
				}
				else
				{
					c0.Play();
					OutputDebugStringW(L"Play\n");
				}
			}

			c0.masterVolume = (float)GetAnalog0(0) / 255.0f;

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

	OutputDebugStringW(L"오디오 스레드가 종료되었습니다.\n");
	return 0;
}