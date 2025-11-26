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
	hidMessageQueues[DJSW_HID_MESSAGE_QUEUE_IDX_AUDIO].bypass = false;

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

	_channel0.Load(L"C:\\Test\\habibi.wav");
	_channel1.Load(L"C:\\Test\\bangalore.wav");
}

static void GlobalCueButtonAction(
	HidMessage* msg,
	int32_t shiftMask,
	int32_t channel,
	int32_t index,
	bool quantize)
{
	AudioChannel* pChannel = GetAudioChannel(channel);

	if (pChannel->GetSource()->IsPlaying())
	{
		if (msg->message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			pChannel->GetSource()->Pause();
			pChannel->GetSource()->Jump(pChannel->GetSource()->GetGlobalCueIndex());
		}
	}
	else
	{
		if (msg->message == DJSW_HID_MASK_MESSAGE_KEY_UP)
		{
			pChannel->GetSource()->PauseGlobalCue();
			pChannel->GetSource()->Jump(pChannel->GetSource()->GetGlobalCueIndex());
		}
		if (msg->message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			if (pChannel->GetSource()->GetPosition() == pChannel->GetSource()->GetGlobalCueIndex())
			{
				pChannel->GetSource()->PlayGlobalCue();
			}
			else
			{
				int32_t position = pChannel->GetSource()->GetPosition();

				if (position < 0)
					position = 0;

				pChannel->GetSource()->SetGlobalCueIndex(position);
				pChannel->GetSource()->Jump(pChannel->GetSource()->GetGlobalCueIndex());
			}
		}
	}
}

static void PadButtonAction(
	HidMessage* msg,
	int32_t shiftMask,
	int32_t channel,
	int32_t index,
	int32_t loopBarCount,
	bool quantize)
{
	AudioChannel* pChannel = GetAudioChannel(channel);

	if (msg->message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
	{
		switch (pChannel->padNumber)
		{
		case 0:
			// Do anything.
			break;
		case 1: // Hot Cue Performance
			if ((msg->modifier & shiftMask) != 0)
			{
				pChannel->GetSource()->ClearHotCue(index);
			}
			else if (pChannel->GetSource()->GetHotCue(index) < 0)
			{
				pChannel->GetSource()->SetHotCue(index);
				//OutputDebugStringW((L"_metaFile.hotCueIndices[0] - 0 == " + to_wstring(pChannel->GetSource()->GetHotCue(0)) + L"\n").c_str());
			}
			else
			{
				int32_t position = pChannel->GetSource()->GetHotCue(index);
				pChannel->GetSource()->Jump(position);
				pChannel->GetSource()->Play();
				//OutputDebugStringW((L"_metaFile.hotCueIndices[0] - 1 == " + to_wstring(pChannel->GetSource()->GetHotCue(0)) + L"\n").c_str());
			}
			break;
		case 2: // Loop Effect
			pChannel->GetSource()->SetLoop(loopBarCount, quantize);
			break;
		case 3: // FX Effect
			if (pChannel->fxNumber == index + 1)
				pChannel->fxNumber = 0;
			else
				pChannel->fxNumber = index + 1;
			break;
		case 4:
			// Do anything.
			break;
		default:
			assert(false);
			break;
		}
	}
}

static void TimeShiftButtonAction(
	HidMessage* msg,
	int32_t shiftMask,
	int32_t channel,
	int32_t index,
	int32_t shiftSamples)
{
	AudioChannel* pChannel = GetAudioChannel(channel);

	int direction = 1;

	if ((msg->modifier & shiftMask) != 0)
		direction = -1;

	if (msg->message == DJSW_HID_MASK_MESSAGE_KEY_PRESS)
	{
		pChannel->GetSource()->SetTimeShiftDistance(shiftSamples * direction);
	}
}

static void AudioInput_Digital(HidMessage msg)
{
	switch (msg.hidKey)
	{
	case DJSW_HID_PLAY1:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			if (_channel0.GetSource()->IsPlaying())
				_channel0.GetSource()->Pause();
			else
				_channel0.GetSource()->Play();
		}
		break;
	case DJSW_HID_PLAY2:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			if (_channel1.GetSource()->IsPlaying())
				_channel1.GetSource()->Pause();
			else
				_channel1.GetSource()->Play();
		}
		break;
	case DJSW_HID_CUE1:
		GlobalCueButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			0,
			false);
		break;
	case DJSW_HID_CUE2:
		GlobalCueButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			0,
			false);
		break;
	case DJSW_HID_PADFN11:
	case DJSW_HID_PADFN12:
	case DJSW_HID_PADFN13:
	case DJSW_HID_PADFN14:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			int number = msg.hidKey - DJSW_HID_PADFN11 + 1;

			_channel0.fxNumber = 0;

			if (_channel0.padNumber == number)
				_channel0.padNumber = 0;
			else
				_channel0.padNumber = number;

			OutputDebugStringW((L"padNumber0 == " + to_wstring(_channel0.padNumber) + L"\n").c_str());
		}
		break;
	case DJSW_HID_PADFN21:
	case DJSW_HID_PADFN22:
	case DJSW_HID_PADFN23:
	case DJSW_HID_PADFN24:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			int number = msg.hidKey - DJSW_HID_PADFN21 + 1;

			_channel1.fxNumber = 0;

			if (_channel1.padNumber == number)
				_channel1.padNumber = 0;
			else
				_channel1.padNumber = number;

			OutputDebugStringW((L"padNumber1 == " + to_wstring(_channel1.padNumber) + L"\n").c_str());
		}
		break;
	case DJSW_HID_PAD11:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			0,
			DJSW_BAR_COUNT_1,
			false);
		break;
	case DJSW_HID_PAD12:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			1,
			DJSW_BAR_COUNT_2,
			false);
		break;
	case DJSW_HID_PAD13:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			2,
			DJSW_BAR_COUNT_4,
			false);
		break;
	case DJSW_HID_PAD14:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			3,
			DJSW_BAR_COUNT_8,
			false);
		break;
	case DJSW_HID_PAD15:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			4,
			DJSW_BAR_COUNT_2_INVERSE,
			false);
		break;
	case DJSW_HID_PAD16:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			5,
			DJSW_BAR_COUNT_4_INVERSE,
			false);
		break;
	case DJSW_HID_PAD17:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			6,
			DJSW_BAR_COUNT_8_INVERSE,
			false);
		break;
	case DJSW_HID_PAD18:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			7,
			DJSW_BAR_COUNT_16_INVERSE,
			false);
		break;
	case DJSW_HID_PAD21:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			0,
			DJSW_BAR_COUNT_1,
			false);
		break;
	case DJSW_HID_PAD22:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			1,
			DJSW_BAR_COUNT_2,
			false);
		break;
	case DJSW_HID_PAD23:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			2,
			DJSW_BAR_COUNT_4,
			false);
		break;
	case DJSW_HID_PAD24:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			3,
			DJSW_BAR_COUNT_8,
			false);
		break;
	case DJSW_HID_PAD25:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			4,
			DJSW_BAR_COUNT_2_INVERSE,
			false);
		break;
	case DJSW_HID_PAD26:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			5,
			DJSW_BAR_COUNT_4_INVERSE,
			false);
		break;
	case DJSW_HID_PAD27:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			6,
			DJSW_BAR_COUNT_8_INVERSE,
			false);
		break;
	case DJSW_HID_PAD28:
		PadButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			7,
			DJSW_BAR_COUNT_16_INVERSE,
			false);
		break;
	case DJSW_HID_TSH11:
		TimeShiftButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			0,
			1
		);
		break;
	case DJSW_HID_TSH12:
		TimeShiftButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			1,
			10
		);
		break;
	case DJSW_HID_TSH13:
		TimeShiftButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			2,
			100
		);
		break;
	case DJSW_HID_TSH14:
		TimeShiftButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_LEFT_SHIFT,
			0,
			3,
			1000
		);
		break;
	case DJSW_HID_TSH21:
		TimeShiftButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			0,
			1
		);
		break;
	case DJSW_HID_TSH22:
		TimeShiftButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			1,
			10
		);
		break;
	case DJSW_HID_TSH23:
		TimeShiftButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			2,
			100
		);
		break;
	case DJSW_HID_TSH24:
		TimeShiftButtonAction(
			&msg,
			DJSW_HID_MASK_MODIFIER_RIGHT_SHIFT,
			1,
			3,
			1000
		);
		break;
	case DJSW_HID_MUTE1:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			_channel0.mute = 1.0f - _channel0.mute;
		}
		break;
	case DJSW_HID_MUTE2:
		if (msg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
		{
			_channel1.mute = 1.0f - _channel1.mute;
		}
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

		float xFaderValue1 = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_CROSSFADER].analogValueFloat;
		float xFaderValue0 = 1.0f - xFaderValue1;
		float xFaderThreshold = 0.35f;
		
		if (xFaderValue0 < xFaderThreshold)
			_channel0.crossVolume = xFaderValue0 / xFaderThreshold;
		else
			_channel0.crossVolume = 1.0f;
		if (xFaderValue1 < xFaderThreshold)
			_channel1.crossVolume = xFaderValue1 / xFaderThreshold;
		else
			_channel1.crossVolume = 1.0f;

		float tmpValue0 = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_TEMPO1].analogValueFloat;
		float tmpValue1 = _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_TEMPO2].analogValueFloat;

		tmpValue0 = 1.0f - 2.0f * tmpValue0;
		tmpValue1 = 1.0f - 2.0f * tmpValue1;

		float tmpRange = (float)(DJSW_WSOLA_TEMPO_RANGE);
		
		_channel0.GetSource()->SetHopDistance((int32_t)(tmpRange * tmpValue0) * 2);
		_channel1.GetSource()->SetHopDistance((int32_t)(tmpRange * tmpValue1) * 2);

		_channel0.fx1 = 1.0f - _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO1].analogValueFloat;
		_channel0.fx2 = 1.0f - _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_FX1].analogValueFloat;

		_channel1.fx1 = 1.0f - _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_EQ_LO2].analogValueFloat;
		_channel1.fx2 = 1.0f - _analogValues[DJSW_IDX_ANALOG_INTERPOLATION_FX2].analogValueFloat;
		
		_channel0.Read(isamples);
		_channel1.Read(isamples + 2);

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

AudioChannel* GetAudioChannel(int index)
{
	switch (index)
	{
	case 0:
		return &_channel0;
	case 1:
		return &_channel1;
	default:
		return NULL;
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
