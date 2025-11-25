#pragma once

#include <immintrin.h>
#include <stdint.h>
#include <Windows.h>

#include "audiofilter.h"
#include "audioreverb.h"
#include "djsw_audiosource.h"
#include "djsw_file_wav.h"
#include "djsw_file_metadata.h"

//#define DJSW_WSOLA_TOLERANCE_RANGE 20 // even number, prefer 10%~30% of overlap size, fixed tolerance range.
#define DJSW_WSOLA_TEMPO_RANGE 5

// stereo channel
class AudioChannel
{
public:
	AudioChannel();

	bool Load(wstring wavFilePath);
	bool Unload();

	void Read(int16_t* out);

	// Pad FX
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
	float fx1;
	float fx2;

	djAudioSource* GetSource();

private:
	djAudioSource _audioSource;
};

void InitAudioChannel();