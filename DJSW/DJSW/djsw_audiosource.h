#pragma once

#include <string>

#include "djsw_file_metadata.h"
#include "djsw_file_wav.h"

using namespace std;

#define DJSW_BAR_COUNT_0 0
#define DJSW_BAR_COUNT_1 1
#define DJSW_BAR_COUNT_2 2
#define DJSW_BAR_COUNT_4 4
#define DJSW_BAR_COUNT_8 8
#define DJSW_BAR_COUNT_16 16
#define DJSW_BAR_COUNT_32 32
#define DJSW_BAR_COUNT_64 64

#define DJSW_BAR_COUNT_2_INVERSE -2
#define DJSW_BAR_COUNT_4_INVERSE -4
#define DJSW_BAR_COUNT_8_INVERSE -8
#define DJSW_BAR_COUNT_16_INVERSE -16
#define DJSW_BAR_COUNT_32_INVERSE -32
#define DJSW_BAR_COUNT_64_INVERSE -64

#define _DJSW_WSOLA_FRAME_SIZE 256 // even number, prefer n-power of 2.
#define _DJSW_WSOLA_OVERLAP_SIZE 128 // even number, MUST BE half of frame size
#define _DJSW_WSOLA_MAX_TOLERANCE 20

class djAudioSource
{
public:
	wstring CreateMetadata(wstring wavPath);
	bool Load(wstring metadataPath);
	bool Unload();

	int32_t GetGlobalCueIndex();
	void SetGlobalCueIndex(int32_t index);
	
	void SetHotCueIndex(int32_t index);
	void SetLoop(int32_t loopBarCount, bool shouldQuantize);

	void SetTempoWeight(float weight);
	void SetTempoRange(float tempoRange);
	void SetTimeShift(int32_t timeShiftSamples);

	void ReadInit();
	void ReadSingle(int16_t* out);
	void Read(int16_t* out);

private:
	djWavMetaFile* _metaFile;
	djWavFileHeader* _header;
	int32_t _audioFileSize;

	int32_t _numWavSamples;
	int16_t* _wavSamples;

	int32_t _glbPosition;
	int32_t _olaPosition;

	bool _useGlobalCue;
	int32_t _glbCueIndex;

	bool _useHotCue;
	int32_t _hotCueIndex;

	bool _shouldJump;
	int32_t _jumpIndex;

	bool _useLoop;
	int32_t _loopIndex;
	int32_t _loopLength;

	int32_t _hopDistance;
	int32_t _tshDistance;

	int32_t _wsolaInputSize;
	int32_t _wsolaSelectedTolerance;
	int16_t* _wsolaInputBuffer;
	void LoadInputBuffer(int32_t hop);

	int32_t _wsolaOutputSize;
	int16_t* _wsolaHannedValueBuffer;
	int16_t* _wsolaHanningWindowBuffer;
	void ApplyHanningWindow(int16_t* buffer, int32_t length);
	int16_t* _wsolaOutputBuffer;
	float GetCrossCorrelation(int16_t* a, int16_t* b);
	int32_t SeekBestOverlapPosition(int32_t tolerance);

	int32_t _xFadeSampleLength = 100; // Tunable.
	int32_t _xFadeBeg;
	int32_t _xFadeSampleLeft;
};