#pragma once

#include <immintrin.h>
#include <stdint.h>
#include <Windows.h>

#include "audiofilter.h"
#include "audioreverb.h"
#include "djsw_audiosource.h"
#include "djsw_file_wav.h"
#include "djsw_file_metadata.h"

// stereo channel
class AudioChannel
{
public:
	AudioChannel();

	bool Load(wstring wavFilePath);
	bool Unload();

	void Read(int16_t* out);

	// Pad FX
	int32_t padNumber;
	int32_t fxNumber;
	SchroederReverb fxReverbL;
	SchroederReverb fxReverbR;

	// EQ
	Biquad eqLowL;
	Biquad eqLowR;
	Biquad eqMidL;
	Biquad eqMidR;
	Biquad eqHighL;
	Biquad eqHighR;

	// Master Volume
	float masterVolume;
	float crossVolume;
	float mute;
	float fx1;
	float fx2;

	djAudioSource* GetSource();

private:
	djAudioSource _audioSource;
};

void InitAudioChannel();