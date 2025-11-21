#include "LoopAudio.h"

#include "djsw_audio_api.h"
#include "djsw_input_hid.h"
#include "djsw_input_hid_controls.h"

static AudioDevice _audioDevice;
static AudioChannel _channel0;
static AudioChannel _channel1;

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

	InitAudioChannel();
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
	//_channel0.Play();
	//_channel1.Play();
}

static void AudioInput_Digital(HidMessage msg)
{
	int lShift = ((msg.modifier & DJSW_HID_MASK_MODIFIER_LEFT_SHIFT) != 0) ? -1 : 1;
	int rShift = ((msg.modifier & DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT) != 0) ? -1 : 1;

	switch (msg.hidKey)
	{
	case DJSW_HID_PLAY1:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			if (_channel0.isPlaying)
				_channel0.Pause();
			else
				_channel0.Play();
		}
		break;
	case DJSW_HID_PLAY2:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			if (_channel1.isPlaying)
				_channel1.Pause();
			else
				_channel1.Play();
		}
		break;
	case DJSW_HID_PADFN11:
	case DJSW_HID_PADFN12:
	case DJSW_HID_PADFN13:
	case DJSW_HID_PADFN14:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			int number = DJSW_HID_PADFN11 - DJSW_HID_PADFN11 + 1;

			if (_channel0.fxNumber == number)
				_channel0.fxNumber = 0;
			else
				_channel0.fxNumber = number;
		}
		break;
	case DJSW_HID_PADFN21:
	case DJSW_HID_PADFN22:
	case DJSW_HID_PADFN23:
	case DJSW_HID_PADFN24:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			int number = msg.hidKey - DJSW_HID_PADFN21 + 1;

			if (_channel1.fxNumber == number)
				_channel1.fxNumber = 0;
			else
				_channel1.fxNumber = number;
		}
		break;
	case DJSW_HID_PAD11:
	case DJSW_HID_PAD12:
	case DJSW_HID_PAD13:
	case DJSW_HID_PAD14:
	case DJSW_HID_PAD15:
	case DJSW_HID_PAD16:
	case DJSW_HID_PAD17:
	case DJSW_HID_PAD18:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_PRESS)
		{
			int number = msg.hidKey - DJSW_HID_PAD11 + 1;

			// TODO: Pad 액션을 구현합니다.
		}
		break;
	case DJSW_HID_PAD21:
	case DJSW_HID_PAD22:
	case DJSW_HID_PAD23:
	case DJSW_HID_PAD24:
	case DJSW_HID_PAD25:
	case DJSW_HID_PAD26:
	case DJSW_HID_PAD27:
	case DJSW_HID_PAD28:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_PRESS)
		{
			int number = msg.hidKey - DJSW_HID_PAD21 + 1;

			// TODO: Pad 액션을 구현합니다.
		}
		break;
	case DJSW_HID_TSH11:
		_channel0.tshDistance = 2 * lShift;
		break;
	case DJSW_HID_TSH12:
		_channel0.tshDistance = 10 * lShift;
		break;
	case DJSW_HID_TSH13:
		_channel0.tshDistance = 100 * lShift;
		break;
	case DJSW_HID_TSH14:
		_channel0.tshDistance = 1000 * lShift;
		break;
	case DJSW_HID_TSH21:
		_channel1.tshDistance = 2 * rShift;
		break;
	case DJSW_HID_TSH22:
		_channel1.tshDistance = 10 * rShift;
		break;
	case DJSW_HID_TSH23:
		_channel1.tshDistance = 100 * rShift;
		break;
	case DJSW_HID_TSH24:
		_channel1.tshDistance = 1000 * rShift;
		break;
	}
}

static void AudioInput_Analog()
{
	if (IsHidConnected())
	{
		_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_CROSSFADER].analogValueInt = GetAnalogMixer(DJSW_IDX_CROSSFADER);

		_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_VOLUME1].analogValueInt = GetAnalogDeck1(DJSW_IDX_VOLUME);
		_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_VOLUME2].analogValueInt = GetAnalogDeck2(DJSW_IDX_VOLUME);

		_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_TEMPO1].analogValueInt = GetAnalogDeck1(DJSW_IDX_TEMPO);
		_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_TEMPO2].analogValueInt = GetAnalogDeck2(DJSW_IDX_TEMPO);

		_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO1].analogValueInt = GetAnalogDeck1(DJSW_IDX_EQ_LO);
		_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_FX1].analogValueInt = GetAnalogDeck1(DJSW_IDX_FX);

		_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO2].analogValueInt = GetAnalogDeck2(DJSW_IDX_EQ_LO);
		_analogValues[DJSW_IDX_ANALOG_INTERPOLATION_FX2].analogValueInt = GetAnalogDeck2(DJSW_IDX_FX);
	}

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

		HidMessage msg;

		while (hidMessageQueues[DJSW_HID_MESSAGE_QUEUE_IDX_AUDIO].Pop(&msg))
		{
			AudioInput_Digital(msg);
		}

		AudioInput_Analog();

		_channel0.masterVolume = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_VOLUME1].analogValueFloat;
		_channel1.masterVolume = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_VOLUME2].analogValueFloat;

		float xFaderValue0 = 1.0f - _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_CROSSFADER].analogValueFloat;
		float xFaderValue1 = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_CROSSFADER].analogValueFloat;

		if (xFaderValue0 < 0.25f)
			_channel0.masterVolume *= xFaderValue0;
		if (xFaderValue1 < 0.25f)
			_channel1.masterVolume *= xFaderValue1;

		float tmpValue0 = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_TEMPO1].analogValueFloat;
		float tmpValue1 = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_TEMPO2].analogValueFloat;

		tmpValue0 = 2.0f * tmpValue0 - 1.0f;
		tmpValue1 = 2.0f * tmpValue1 - 1.0f;

		float tmpRange = (float)(DJSW_WSOLA_TEMPO_RANGE);
		_channel0.hopDistance = (int32_t)(tmpRange * tmpValue0) * 2; // should be odd number
		_channel1.hopDistance = (int32_t)(tmpRange * tmpValue1) * 2; // should be odd number
		
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

int32_t GetPosition(int channel)
{
	switch (channel)
	{
	case 0:
		return _channel0.position;
	case 1:
		return _channel1.position;
	default:
		return -1;
	}
}

void PeekSample(int16_t* out, int channel, int32_t position)
{
	switch (channel)
	{
	case 0:
		out[0] = _channel0.wavSamples[position];
		out[1] = _channel0.wavSamples[position + 1];
		return;
	case 1:
		out[0] = _channel1.wavSamples[position];
		out[1] = _channel1.wavSamples[position + 1];
		return;
	default:
		out[0] = 0;
		out[1] = 0;
		return;
	}
}

bool IsAudioLoaded(int channel)
{
	switch (channel)
	{
	case 0:
		return _channel0.IsLoaded();
	case 1:
		return _channel1.IsLoaded();
	default:
		return false;
	}
}

// -------------------- LoopAudio.h implementations --------------------

DWORD WINAPI AudioMain(LPVOID lpParams)
{
	AudioParams* audioParams = (AudioParams*)lpParams;

	AudioInit();

	// 초기화 로직 동기화를 위함.
	audioParams->loopBaseParams.interruptNumber = DJSW_INT_NULL;

	while (audioParams->loopBaseParams.interruptNumber != 1)
	{
		AudioUpdate();
	}

	AudioFinal();

	return 0;
}
