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

class djAnalyzeJob
{
public:
	djWavGridData result;

	djAnalyzeJob();
	djAnalyzeJob(
		char* wavFilePath,
		float bpmMin,
		float bpmMax,
		float bpmUnit);

	bool Init();
	bool Analyze();
	bool Final();
	void Release();

private:
	// parameters
	float _bpmMin;
	float _bpmMax;
	float _bpmUnit;
	char* _wavFilePath;

	// iterators
	float _bpm;
	float _offset;
	int32_t _samplesPerBar;

	djWavFileHeader* _header;
	int16_t* _samples;
	int32_t _sampleCount;
	int32_t _sampleRate;
	int32_t _numChannels;

	float _optAutoCorrelation;
	float _optBpm;
	int32_t _optOffset;

	float _maxAutoCorrelation;
	float _maxOffset;
};

int32_t GetSamplesPerBar(int32_t sampleRate, int32_t channelCount, float bpm);