#include "djsw_file_metadata.h"

#include <assert.h>
//#include <Shlwapi.h>

djWavMetaFile::djWavMetaFile() :
	_metadata({ 0 }),
	_fileHandle(INVALID_HANDLE_VALUE),
	_dataChangeCount(0)
{
	Init();
}

void djWavMetaFile::Init()
{
	_metadata.defaultGridData.bpm = 0.0f;
	_metadata.defaultGridData.barCount = -1;

	for (int i = 0; i < DJSW_MAX_HOT_CUE_COUNT; ++i)
	{
		_metadata.hotCueIndices[i] = -1;
	}
}

bool djWavMetaFile::Open(wstring metaFilePath)
{
	LPCWSTR filePath = metaFilePath.c_str();
	
	HANDLE fileHandle = CreateFileW(
		filePath,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (fileHandle == INVALID_HANDLE_VALUE)
		return false;

	DWORD err = GetLastError();

	DWORD rdLength = sizeof(_metadata);
	DWORD bytesRead = 0;

	switch (err)
	{
	case 0: // File create new.
		_fileHandle = fileHandle;
		return Save();
	case ERROR_ALREADY_EXISTS:
		_fileHandle = fileHandle;

		if (SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			return false;

		if (!ReadFile(fileHandle, &_metadata, rdLength, &bytesRead, NULL))
			return false;

		return bytesRead == rdLength;
	default:
		return false;
	}
}

bool djWavMetaFile::Save()
{
	djWavMetadata data = _metadata;
	DWORD wrLength = sizeof(data);
	DWORD bytesWrite = 0;

	if (SetFilePointer(_fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return false;

	if (!WriteFile(_fileHandle, &data, wrLength, &bytesWrite, NULL))
		return false;

	return bytesWrite == wrLength;
}

bool djWavMetaFile::Close()
{
	Save();

	if (_fileHandle == INVALID_HANDLE_VALUE)
		return false;

	if (!CloseHandle(_fileHandle))
		return false;

	_fileHandle = INVALID_HANDLE_VALUE;
	return true;
}

void djWavMetaFile::SetDefaultGridData(djWavGridData* data)
{
	_metadata.defaultGridData = *data;
}

bool djWavMetaFile::SetWavFile(wstring wavFilePath)
{
	LPCWSTR filePath = wavFilePath.c_str();

	//if (!PathFileExistsW(filePath)) return false;

	int len = WideCharToMultiByte(
		CP_UTF8, // UTF-8로 변환
		0,
		filePath,
		-1, // null 포함
		_metadata.wavFilePath,
		MAX_PATH,
		NULL,
		NULL
	);

	if (len == 0)
		return false;

	return true;
}

char* djWavMetaFile::GetWavFilePath()
{
	return _metadata.wavFilePath;
}

float djWavMetaFile::GetBpm()
{
	return _metadata.defaultGridData.bpm;
}

int32_t djWavMetaFile::GetFirstBarIndex()
{
	return _metadata.defaultGridData.firstBarIndex;
}

void djWavMetaFile::SetHotCueIndex(int hotCueNumber, int32_t index)
{
	assert(hotCueNumber >= 0 && hotCueNumber < DJSW_MAX_HOT_CUE_COUNT);

	_metadata.hotCueIndices[hotCueNumber] = index;
}