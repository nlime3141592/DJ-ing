#include "audiochannel.h"

#include <assert.h>

#define DJSW_EQ_LOW_F 200.0f
#define DJSW_EQ_MID_F 1000.0f
#define DJSW_EQ_HIGH_F 5000.0f

AudioChannel::AudioChannel() :
	padNumber(0),
	fxNumber(0),
	fxReverbL(SchroederReverb()),
	fxReverbR(SchroederReverb()),

	eqLowL(Biquad()),
	eqLowR(Biquad()),
	eqMidL(Biquad()),
	eqMidR(Biquad()),
	eqHighL(Biquad()),
	eqHighR(Biquad()),

	masterVolume(0.0f),
	mute(1.0f),
	fx1(0.0f),
	fx2(0.0f)
{
	float fs = 44100.0f;
	
	InitParamsLPF(&eqLowL, DJSW_EQ_LOW_F, fs);
	InitParamsLPF(&eqLowR, DJSW_EQ_LOW_F, fs);
	InitParamsBPF(&eqMidL, DJSW_EQ_MID_F, fs);
	InitParamsBPF(&eqMidR, DJSW_EQ_MID_F, fs);
	InitParamsHPF(&eqHighL, DJSW_EQ_HIGH_F, fs);
	InitParamsHPF(&eqHighR, DJSW_EQ_HIGH_F, fs);

	SetParamsLPF_Safe(&eqLowL, INV_SQRT_2, DJSW_EQ_LOW_F, fs);
	SetParamsLPF_Safe(&eqLowR, INV_SQRT_2, DJSW_EQ_LOW_F, fs);
	SetParamsBPF_Safe(&eqMidL, INV_SQRT_2, DJSW_EQ_MID_F, fs);
	SetParamsBPF_Safe(&eqMidR, INV_SQRT_2, DJSW_EQ_MID_F, fs);
	SetParamsHPF_Safe(&eqHighL, INV_SQRT_2, DJSW_EQ_HIGH_F, fs);
	SetParamsHPF_Safe(&eqHighR, INV_SQRT_2, DJSW_EQ_HIGH_F, fs);
}

bool AudioChannel::Load(wstring wavFilePath)
{
	_audioSource = djAudioSource();
	wstring metaFilePath = _audioSource.CreateMetadata(wavFilePath);
	_audioSource.Load(metaFilePath);
	//_audioSource.Load(wavFilePath + L".djmeta");
	return true;
}

bool AudioChannel::Unload()
{
	return false;
}

static float MapExp(float t, float min, float max)
{
	return min * powf(max / min, t);
}

void AudioChannel::Read(int16_t* out)
{
	_audioSource.Read(out);

	float fs = 44100.0f;
	float fLow = MapExp(1.0f - fx1, DJSW_EQ_LOW_F, 20000.0f);
	float qMid = MapExp(fx2, 0.3f, 10.0f);
	float fHigh = MapExp(fx2, 20.0f, DJSW_EQ_HIGH_F);
	
	SetParamsLPF_Safe(&eqLowL, INV_SQRT_2, fLow, fs);
	SetParamsLPF_Safe(&eqLowR, INV_SQRT_2, fLow, fs);
	out[0] = BiquadConvolution(&eqLowL, out[0]);
	out[1] = BiquadConvolution(&eqLowR, out[1]);

	switch (fxNumber)
	{
	case 0:
		break;
	case 1:
		SetParamsBPF_Safe(&eqMidL, qMid, DJSW_EQ_MID_F, fs);
		SetParamsBPF_Safe(&eqMidR, qMid, DJSW_EQ_MID_F, fs);
		out[0] = BiquadConvolution(&eqMidL, out[0]);
		out[1] = BiquadConvolution(&eqMidR, out[1]);
		break;
	case 2:
		SetParamsHPF_Safe(&eqHighL, INV_SQRT_2, fHigh, fs);
		SetParamsHPF_Safe(&eqHighR, INV_SQRT_2, fHigh, fs);
		out[0] = BiquadConvolution(&eqHighL, out[0]);
		out[1] = BiquadConvolution(&eqHighR, out[1]);
		break;
	case 3:
		fxReverbL.SetWet(fx2);
		fxReverbR.SetWet(fx2);
		out[0] = fxReverbL.Process(out[0]);
		out[1] = fxReverbR.Process(out[1]);
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	default:
		break;
	}

	out[0] *= masterVolume * mute;
	out[1] *= masterVolume * mute;
}

djAudioSource* AudioChannel::GetSource()
{
	return &_audioSource;
}

void InitAudioChannel()
{
	// Do anything.
}