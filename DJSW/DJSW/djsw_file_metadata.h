#pragma once

#include <stdint.h>
#include <string>
#include <Windows.h>

#define DJSW_MAX_HOT_CUE_COUNT 16
#define DJSW_MAX_CUSTOM_GRID_COUNT 64
#define DJSW_MAX_CUSTOM_GRID_INDEX_COUNT 256

#define DJSW_DATA_AUTO_SAVE_PERIOD 64

using namespace std;

struct djWavGridData
{
	float bpm;
	int32_t samplesPerBar;
	int32_t firstBarIndex;
	int32_t barCount;
};

struct djWavMetadata
{
	djWavGridData defaultGridData;

	char wavFilePath[MAX_PATH];

	int32_t hotCueIndices[DJSW_MAX_HOT_CUE_COUNT];

	int32_t customGridCount;
	djWavGridData customGrids[DJSW_MAX_CUSTOM_GRID_COUNT];

	int32_t customGridIndexCount;
	int32_t customGridIndices[DJSW_MAX_CUSTOM_GRID_INDEX_COUNT];
};

class djWavMetaFile
{
public:
	djWavMetaFile();

	void Init();
	bool Open(wstring metaFilePath);
	bool Save();
	bool Close();

	float GetBpm();
	int32_t GetSamplesPerBar();
	int32_t GetFirstBarIndex();
	
	void SetHotCueIndex(int hotCueNumber, int32_t index);

private:
	djWavMetadata _metadata;
	HANDLE _fileHandle;
	int _dataChangeCount;
};