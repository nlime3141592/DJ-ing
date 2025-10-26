#pragma once

#include <stdint.h>
#include <Windows.h>

#include "audiofilter.h"
#include "audioreverb.h"
#include "pcmwav.h"

// stereo channel
class AudioChannel
{
public:
	AudioChannel();

	bool Load(const char* fileName);
	bool Unload();

	void Reset();

	void Play();
	void Pause();

	bool Read(int16_t* lSampleOutput, int16_t* rSampleOutput);

	void Jump(int32_t sampleJumpingTo, int32_t whatSampleJumpingFrom);
	void JumpImmediate(int32_t sampleJumpingTo);

	void SetLoop(int32_t begin, int32_t length);
	void ClearLoop();

	void OnInputDigital(uint8_t* hidKeyReport);
	void OnInputAnalog(uint8_t* hidKeyReport);










	// Wave File Data
	void* fileData;
	WavFile_PCM* data;
	uint32_t fileSize;

	// Metadata for Audio Source
	//int32_t cuePoint;

	// Sample Data
	uint32_t numWavSamples;
	uint16_t* wavSamples;
	int32_t position;
	bool isPlaying;
	bool isMuted;

	// Jump
	int32_t xFadeSampleLength = 100; // Tunable
	int32_t xFadeBeg;
	int32_t xFadeSampleLeft;

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

	// Loop
	int32_t loopBeg;
	int32_t loopLength;

	// Master Volume
	float masterVolume;

private:
	
};