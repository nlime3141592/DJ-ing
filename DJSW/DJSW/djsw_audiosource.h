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

class djAudioSource
{
public:
	wstring CreateMetadata(wstring wavPath);
	bool Load(wstring metadataPath);
	bool Unload();

	int32_t GetGlobalCueIndex();
	int32_t SetGlobalCueIndex();
	
	void SetHotCueIndex(int32_t index);
	void SetLoop(int32_t loopBarCount, bool shouldQuantize);

	void SetTempoWeight(float weight);
	void SetTempoRange(float tempoRange);
	void SetTimeShift(int32_t timeShiftSamples);

private:
	int32_t _glbCueIndex;
	int32_t _hotCueIndex;
	int32_t _loopIndex;
	int32_t _loopLength;
};