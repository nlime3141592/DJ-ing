#include "audiochannel.h"

#include <assert.h>

AudioChannel::AudioChannel() :
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
	fx1(0.0f),
	fx2(0.0f)
{
	float fs = 44100.0f;
	eqLowL.fs = fs;
	eqLowR.fs = fs;
	eqMidL.fs = fs;
	eqMidR.fs = fs;
	eqHighL.fs = fs;
	eqHighR.fs = fs;

	float fLow = 200.0f;
	float fMid = 1000.0f;
	float fHigh = 5000.0f;
	eqLowL.f = fLow;
	eqLowR.f = fLow;
	eqMidL.f = fMid;
	eqMidR.f = fMid;
	eqHighL.f = fHigh;
	eqHighR.f = fHigh;
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

void AudioChannel::Read(int16_t* out)
{
	_audioSource.Read(out);

	out[0] = (int16_t)((float)out[0] * masterVolume);
	out[1] = (int16_t)((float)out[1] * masterVolume);

	eqLowL.gain = fx1;
	eqLowR.gain = fx1;
	out[0] = BiquadConvolution(&eqLowL, out[0]);
	out[1] = BiquadConvolution(&eqLowR, out[1]);

	switch (fxNumber)
	{
	case 0:
		break;
	case 1:
		eqMidL.gain = fx2;
		eqMidR.gain = fx2;
		out[0] = BiquadConvolution(&eqMidL, out[0]);
		out[1] = BiquadConvolution(&eqMidR, out[1]);
		break;
	case 2:
		eqHighL.gain = fx2;
		eqHighR.gain = fx2;
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
}

djAudioSource* AudioChannel::GetSource()
{
	return &_audioSource;
}

void InitAudioChannel()
{
	// Do anything.
}