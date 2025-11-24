#include "djsw_file_wav.h"

#include <Windows.h>

bool LoadWavFile(char* wavFilePath, djWavFileHeader** header, int32_t* fileSize)
{
	HANDLE audioFile = CreateFileA(
		wavFilePath,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (audioFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD audioFileSize = GetFileSize(audioFile, 0);

	if (!audioFileSize)
		return false;

	void* audioFileData = HeapAlloc(GetProcessHeap(), 0, audioFileSize + 1);

	if (!audioFileData)
		return false;

	if (!ReadFile(audioFile, audioFileData, audioFileSize, &audioFileSize, NULL))
		return false;

	CloseHandle(audioFile);

	// TODO: 왜 마지막에 NULL 바이트를 넣어주는지 이유 파악하기.
	((uint8_t*)audioFileData)[audioFileSize] = 0;

	*header = (djWavFileHeader*)audioFileData;
	*fileSize = audioFileSize;

	return true;
}

bool IsValidWavFile(djWavFileHeader* header)
{
	// Check the Chunk IDs to make sure we loaded the file correctly
	if (header->riffId != 1179011410)
		return false;
	if (header->waveId != 1163280727)
		return false;
	if (header->fmtId != 544501094)
		return false;
	if (header->dataId != 1635017060)
		return false;

	// Check data is in format we expect
	if (header->formatCode != 1) // Only support PCM data
		return false;
	if (header->numChannels != 2) // Only support 2-channel data
		return false;
	if (header->fmtChunkSize != 16) // This should be true for PCM data
		return false;
	if (header->sampleRate != 44100) // Only support 44100Hz data
		return false;
	if (header->bitsPerSample != 16) // Only support 16-bit samples
		return false;

	// This is how these fields are defined, no harm to assert that they're what we expect
	if (header->blockAlign != header->numChannels * header->bitsPerSample / 8)
		return false;
	if (header->byteRate != header->sampleRate * header->blockAlign)
		return false;

	return true;
}