#include "LoopAudio.h"

#include "djsw_audio_api.h"

AudioDevice _audioDevice;
AudioChannel _channel0;
AudioChannel _channel1;

typedef struct
{
	uint8_t analogValueInt;
	float analogValueFloat;
} djAnalogInterpolation;

static djAnalogInterpolation _analogValues[DJSW_MAX_ANALOG_INTERPOLATION_COUNT];

static void AudioAnalogInterpolation(djAnalogInterpolation* interpolation)
{
	float beg = interpolation->analogValueFloat;
	float end = (float)interpolation->analogValueInt / 255.0f;

	float speed = 0.367879441f; // inverse of exponential == 1/e == 0.367879441f
	float next = beg + (end - beg) * speed;

	if (next < 10e-4)
		next = end;

	interpolation->analogValueFloat = next;
}

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

static inline void AnalogInterpolationInit(djAnalogInterpolation* interpolation, uint8_t value)
{
	interpolation->analogValueInt = value;
	interpolation->analogValueFloat = (float)value / 255.0f;
}

static void AudioInit()
{
	_audioDevice = AudioDevice();
	_audioDevice.Init();
	_audioDevice.Start();

	_channel0 = AudioChannel();
	_channel1 = AudioChannel();

	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_CROSSFADER, DJSW_ANALOG_VALUE_MID);

	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_VOLUME1, DJSW_ANALOG_VALUE_MAX);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_VOLUME2, DJSW_ANALOG_VALUE_MAX);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_VOLUME3, DJSW_ANALOG_VALUE_MAX);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_VOLUME4, DJSW_ANALOG_VALUE_MAX);

	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_TEMPO1, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_TEMPO2, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_TEMPO3, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_TEMPO4, DJSW_ANALOG_VALUE_MID);

	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO1, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_MD1, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_HI1, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_FX1, DJSW_ANALOG_VALUE_MID);

	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO2, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_MD2, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_HI2, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_FX2, DJSW_ANALOG_VALUE_MID);

	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO3, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_MD3, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_HI3, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_FX3, DJSW_ANALOG_VALUE_MID);

	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO4, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_MD4, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_HI4, DJSW_ANALOG_VALUE_MID);
	AnalogInterpolationInit(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_FX4, DJSW_ANALOG_VALUE_MID);

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

		_channel0.masterVolume = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_VOLUME1].analogValueFloat;
		_channel1.masterVolume = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_VOLUME2].analogValueFloat;

		float xFaderValue0 = 1.0f - _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_CROSSFADER].analogValueFloat;
		float xFaderValue1 = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_CROSSFADER].analogValueFloat;

		if (xFaderValue0 < 0.25f)
			_channel0.masterVolume *= xFaderValue0;
		if (xFaderValue1 < 0.25f)
			_channel1.masterVolume *= xFaderValue1;

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

// -------------------- LoopAudio.h implementations --------------------
void AudioInput()
{
	// Digital Inputs
	// TODO: 디지털 입력을 이 곳에서 처리합니다.
	
	// Analog Inputs
	_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_CROSSFADER].analogValueInt = GetAnalogMixer(DJSW_IDX_CROSSFADER);

	_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_VOLUME1].analogValueInt = GetAnalogDeck1(DJSW_IDX_VOLUME);
	_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_VOLUME2].analogValueInt = GetAnalogDeck2(DJSW_IDX_VOLUME);

	_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_TEMPO1].analogValueInt = GetAnalogDeck1(DJSW_IDX_TEMPO);
	_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_TEMPO2].analogValueInt = GetAnalogDeck2(DJSW_IDX_TEMPO);

	_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO1].analogValueInt = GetAnalogDeck1(DJSW_IDX_EQ_LO);
	_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_FX1].analogValueInt = GetAnalogDeck1(DJSW_IDX_FX);

	_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO2].analogValueInt = GetAnalogDeck2(DJSW_IDX_EQ_LO);
	_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_FX2].analogValueInt = GetAnalogDeck2(DJSW_IDX_FX);

	// Analog Input Interpolation Logics
	AudioAnalogInterpolation(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_CROSSFADER);

	AudioAnalogInterpolation(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_VOLUME1);
	AudioAnalogInterpolation(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_VOLUME2);

	AudioAnalogInterpolation(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_TEMPO1);
	AudioAnalogInterpolation(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_TEMPO2);

	AudioAnalogInterpolation(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO1);
	AudioAnalogInterpolation(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_FX1);

	AudioAnalogInterpolation(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO2);
	AudioAnalogInterpolation(_analogValues + DJSW_IDX_ANALOG_INTERPOLATION_FX2);
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

// -------------------- djsw_audio_api.h implementations --------------------
void SetAnalogValue(uint8_t value, int index)
{
	_analogValues[index].analogValueInt = value;
}

uint8_t GetAnalogValueInt(int index)
{
	return _analogValues[index].analogValueInt;
}

float GetAnalogValueFloat(int index)
{
	return _analogValues[index].analogValueFloat;
}
