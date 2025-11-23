#pragma once

#include <stdint.h>
#include <string>
#include <Windows.h>

#include "djsw_file_wav.h"

#define DJSW_MAX_HOT_CUE_COUNT 16
#define DJSW_MAX_CUSTOM_GRID_COUNT 64
#define DJSW_MAX_CUSTOM_GRID_INDEX_COUNT 256

#define DJSW_DATA_AUTO_SAVE_PERIOD 64

using namespace std;

struct djWavGridData
{
	float bpm;
	int32_t reserved0; // 마디당 샘플 수 == fs * channel * 240 / bpm
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

	void SetDefaultGridData(djWavGridData* data);
	bool SetWavFile(wstring wavFilePath);
	char* GetWavFilePath();
	
	float GetBpm();
	int32_t GetFirstBarIndex();
	
	void SetHotCue(int hotCueIndex, int32_t position);
	int32_t GetHotCue(int hotCueIndex);

private:
	djWavMetadata _metadata;
	HANDLE _fileHandle;
	int _dataChangeCount;
};