#pragma once

#include <immintrin.h>
#include <stdint.h>
#include <Windows.h>

#include "audiofilter.h"
#include "audioreverb.h"
#include "djsw_audiosource.h"
#include "djsw_file_wav.h"
#include "djsw_file_metadata.h"

#define DJSW_WSOLA_FRAME_SIZE 512 // even number, prefer n-power of 2.
#define DJSW_WSOLA_OVERLAP_SIZE 256 // even number, MUST BE half of frame size
//#define DJSW_WSOLA_TOLERANCE_RANGE 20 // even number, prefer 10%~30% of overlap size, fixed tolerance range.
#define DJSW_WSOLA_TEMPO_RANGE 10

// stereo channel
class AudioChannel
{
public:
	AudioChannel();

	bool IsLoaded();

	bool Load(const char* fileName);
	bool Unload();

	void Reset();

	void Play();
	void Pause();

	void Read16(int16_t* out);
	void Read2(int16_t* out);
	void ReadPass2(int16_t* out);
	void Read(int16_t* out);

	void Jump(int32_t sampleJumpingTo, int32_t whatSampleJumpingFrom);
	void JumpImmediate(int32_t sampleJumpingTo);

	void SetLoop(int32_t begin, int32_t length);
	void ClearLoop();

	// Wave File Data
	void* fileData;
	djWavFileHeader* data;
	uint32_t fileSize;

	// Metadata for Audio Source
	//int32_t cuePoint;

	// Sample Data
	uint32_t numWavSamples;
	int16_t* wavSamples;
	int32_t position;
	int32_t olaPosition;
	bool isPlaying;
	bool isMuted;

	// Jump
	int32_t xFadeSampleLength = 100; // Tunable
	int32_t xFadeBeg;
	int32_t xFadeSampleLeft;

	// Global Cue
	int32_t cuePosition = 0;

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

	int32_t hopDistance;
	int32_t tshDistance;

	djAudioSource* GetSource();
	
private:
	djAudioSource _audioSource;
	
	int32_t _wsolaPrevFrameIndex;
	int16_t _wsolaBuffer[DJSW_WSOLA_FRAME_SIZE];

	void HanningWindow(int16_t* buffer);
	int32_t CrossCorrelation(int16_t* buffer0, int16_t* buffer1, int length);
	int32_t SeekBestOverlapPosition(int32_t toleranceRange);
	void WaveformSimilarityOLA();
};

void InitAudioChannel();