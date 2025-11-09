#include "LoopAudio.h"

AudioDevice _audioDevice;
AudioChannel _channel0;
AudioChannel _channel1;

static inline void MixSample1(const int16_t* s0, const int16_t* s1, int16_t* out)
{
	float fSample0 = (float)*s0;
	float fSample1 = (float)*s1;

	float mix = (fSample0 + fSample1) / 32768.0f;

	// hyper tangent approximation (clipping)
	// tanh(x) = x * (27 + x^2) / (27 + 9x^2)
	float x2 = mix * mix;
	float t = mix * (27.0f + x2) / (27.0f + 9.0f * x2);

	*out = (int16_t)(32767.0f * t);
}

static void AudioInit()
{
	_audioDevice = AudioDevice();
	_audioDevice.Init();
	_audioDevice.Start();

	_channel0 = AudioChannel();
	_channel1 = AudioChannel();

	// TEST: for Debugging.
	bool result1 = _channel0.Load("C:\\Test\\habibi.wav");
	bool result2 = _channel1.Load("C:\\Test\\bangalore.wav");
	assert(result1);
	assert(result2);
	_channel0.Play();
	_channel1.Play();
}

static void AudioUpdate()
{
	// 1. 버퍼 크기 얻기
	UINT32 bufferSizeInFrames;
	HRESULT hr = _audioDevice.audioClient->GetBufferSize(&bufferSizeInFrames);
	assert(hr == S_OK);

	// 2. 패딩 버퍼 확인
	UINT32 bufferPadding;
	hr = _audioDevice.audioClient->GetCurrentPadding(&bufferPadding);
	assert(hr == S_OK);

	// 3. 목표 버퍼 패딩 정의
	const float TARGET_BUFFER_PADDING_IN_SECONDS = 1 / 64.0f;
	UINT32 targetBufferPadding = UINT32(bufferSizeInFrames * TARGET_BUFFER_PADDING_IN_SECONDS);
	UINT32 numFramesToWrite = targetBufferPadding - bufferPadding;

	// 4. 렌더 버퍼 얻기
	int16_t* buffer;
	hr = _audioDevice.audioRenderClient->GetBuffer(numFramesToWrite, (BYTE**)(&buffer));
	assert(hr == S_OK);

	// 5. 샘플 쓰기

	// 4-channel SIMD Processing (AVX2 Instruction Set Needs.)
	for (UINT32 frameIndex = 0; frameIndex < numFramesToWrite; ++frameIndex)
	{
		// TODO: 매 샘플마다 입력 처리가 필요한가?
		float samples[8] = { 0.0f };
		int16_t isamples[8] = { 0 };

		_channel0.masterVolume = (float)GetAnalogDeck1(0) / 255.0f;
		_channel1.masterVolume = (float)GetAnalogDeck2(0) / 255.0f;

		_channel0.Read2(isamples);
		_channel1.Read2(isamples + 2);

		MixSample1(isamples + 0, isamples + 2, buffer + 0);
		MixSample1(isamples + 1, isamples + 3, buffer + 1);
		buffer += 2;
	}

	// 6. 샘플 제출
	hr = _audioDevice.audioRenderClient->ReleaseBuffer(numFramesToWrite, 0);
	assert(hr == S_OK);

	// 7. 재생 타이밍 정보 확인 (선택사항)
	IAudioClock* audioClock;
	_audioDevice.audioClient->GetService(__uuidof(IAudioClock), (LPVOID*)(&audioClock));
	UINT64 audioPlaybackFreq;
	UINT64 audioPlaybackPos;
	audioClock->GetFrequency(&audioPlaybackFreq);
	audioClock->GetPosition(&audioPlaybackPos, 0);
	audioClock->Release();
	// UINT64 audioPlaybackPosInSeconds = audioPlaybackPos/audioPlaybackFreq;
	// UINT64 audioPlaybackPosInSamples = audioPlaybackPosInSeconds*mixFormat.nSamplesPerSec;
}

static void AudioFinal()
{
	_audioDevice.audioClient->Stop();
	_audioDevice.audioClient->Release();
	_audioDevice.audioRenderClient->Release();

	_channel0.Unload();
	_channel1.Unload();
}

DWORD WINAPI AudioMain(LPVOID lpParams)
{
	AudioParams* audioParams = (AudioParams*)lpParams;

	AudioInit();

	while (audioParams->loopBaseParams.interruptNumber != 1)
	{
		AudioUpdate();
	}

	AudioFinal();

	return 0;
}