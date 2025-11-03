#include "audiothread.h"

#include "audiodevice.h"
#include "audiochannel.h"

#include "HIDThread.h"

AudioDevice device;
AudioChannel channel0;

void AudioInit()
{
	device = AudioDevice();
	device.Init();
	device.Start();

	channel0 = AudioChannel();
	bool result = channel0.Load("C:\\Test\\habibi.wav");
	assert(result);

	channel0.Play();
}

void AudioLoop()
{
	UINT32 bufferSizeInFrames;
	HRESULT hr = device.audioClient->GetBufferSize(&bufferSizeInFrames);
	assert(hr == S_OK);

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
			if (channel0.isPlaying)
			{
				channel0.Pause();
				OutputDebugStringW(L"Pause\n");
			}
			else
			{
				channel0.Play();
				OutputDebugStringW(L"Play\n");
			}
		}

		channel0.masterVolume = (float)GetAnalog0(2) / 255.0f;

		int16_t lSample;
		int16_t rSample;

		if (!channel0.Read(&lSample, &rSample))
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

void AudioFinal()
{
	device.audioClient->Stop();
	device.audioClient->Release();
	device.audioRenderClient->Release();

	channel0.Unload();
}