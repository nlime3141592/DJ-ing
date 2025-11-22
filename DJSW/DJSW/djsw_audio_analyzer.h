#pragma once

#include <stdint.h>

#include "djsw_file_metadata.h"

typedef struct djAudioAnalyzerParams
{
	float bpmMin;
	float bpmMax;
	float bpmUnit;
	int32_t sampleCount;
	int32_t sampleRate;
	int32_t channelCount;
	int16_t* samples;
};

int32_t GetSamplesPerBar(int32_t sampleRate, int32_t channelCount, float bpm);

void AnalyzeGridData(
	djAudioAnalyzerParams* params,
	djWavGridData* resultBuffer);