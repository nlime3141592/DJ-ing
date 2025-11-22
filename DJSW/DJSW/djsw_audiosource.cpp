#include "djsw_audiosource.h"

#include <assert.h>

#include "djsw_audio_analyzer.h"

wstring djAudioSource::CreateMetadata(wstring wavPath)
{
	return NULL;
}

bool djAudioSource::Load(wstring metaFilePath)
{
	// 1. Load Meta File
	if (!_metaFile->Open(metaFilePath))
		return false;

	// 2. Load Audio File
	HANDLE audioFile = CreateFileA(
		_metaFile->GetWavFilePath(),
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

	_header = (djWavFileHeader*)audioFileData;
	_audioFileSize = audioFileSize;

	assert(IsValidWavFile(_header));

	// 3. Post processing.
	_numWavSamples = _header->dataChunkSize / sizeof(int16_t);
	_wavSamples = _header->samples;

	_wsolaInputSize = _header->numChannels * sizeof(int16_t) * (_DJSW_WSOLA_FRAME_SIZE + 2 * _DJSW_WSOLA_MAX_TOLERANCE);
	_wsolaInputBuffer = (int16_t*)HeapAlloc(GetProcessHeap(), 0, _wsolaInputSize);

	_wsolaOutputSize = _header->numChannels * sizeof(int16_t) * _DJSW_WSOLA_FRAME_SIZE;
	_wsolaOutputBuffer = (int16_t*)HeapAlloc(GetProcessHeap(), 0, _wsolaOutputSize);

	return true;
}

bool djAudioSource::Unload()
{
	if (_metaFile != NULL)
	{
		_metaFile->Save();
		_metaFile->Close();
		_metaFile = NULL;
	}
	
	if (!HeapFree(GetProcessHeap(), 0, _header))
		return false;

	_header = NULL;
	_audioFileSize = 0;

	_numWavSamples = 0;
	_wavSamples = NULL;

	_glbCueIndex = 0;
	_hotCueIndex = 0;
	_loopIndex = 0;
	_loopLength = 0;

	return true;
}

int32_t djAudioSource::GetGlobalCueIndex()
{
	return _glbCueIndex;
}

void djAudioSource::SetGlobalCueIndex(int32_t index)
{
	_glbCueIndex = index;
}

void djAudioSource::SetHotCueIndex(int32_t index)
{
	_hotCueIndex = index;
}

void djAudioSource::SetLoop(int32_t loopBarCount, bool shouldQuantize)
{
	int32_t samplesPerBar = GetSamplesPerBar(_header->sampleRate, _header->numChannels, _metaFile->GetBpm());

	if (loopBarCount > 0)
	{
		_useLoop = true;
		_loopLength = samplesPerBar * loopBarCount;
	}
	else if (loopBarCount < 0)
	{
		_useLoop = true;
		_loopLength = samplesPerBar / (-loopBarCount);
	}
	else
	{
		_useLoop = false;
		_loopLength = 0;
		_loopIndex = -1;
		return;
	}

	int32_t position = _glbPosition + _olaPosition;

	if (shouldQuantize)
	{
		int32_t idxLeft = _metaFile->GetFirstBarIndex() + position / samplesPerBar;
		int32_t idxRight = idxLeft + samplesPerBar;

		int32_t distLeft = position - idxLeft;
		int32_t distRight = idxRight - position;

		if (distLeft < distRight)
			_loopIndex = idxLeft;
		else
			_loopIndex = idxRight;
	}
	else
	{
		_loopIndex = position;
	}
}

void djAudioSource::SetTempoWeight(float weight)
{

}

void djAudioSource::SetTempoRange(float tempoRange)
{

}

void djAudioSource::SetTimeShift(int32_t timeShiftSamples)
{
	_tshDistance = timeShiftSamples * _header->numChannels;
}

void djAudioSource::ReadInit()
{
	for (int32_t i = 0; i < _wsolaInputSize; ++i)
	{

	}
}

void djAudioSource::ReadSingle(int16_t* out)
{
	// 1. 즉시 점프 발생 (Global/Hot Cue)
	if (_shouldJump)
	{
		_xFadeBeg = _glbPosition + _olaPosition;
		_xFadeSampleLeft = _xFadeSampleLength;

		// 입력 버퍼를 적절히 수정해야 함.
		//memset(_wsolaInputBuffer, ... );

		// 윈도우를 적용하지 않는 신호 샘플
		for (int32_t i = _olaPosition * _header->numChannels; i < _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels; ++i)
		{
			// if, 루프 경계에서 샘플 처리 (출력 버퍼에 바로 저장)
			// if, 크로스페이드 샘플 처리 (출력 버퍼의 값과 크로스페이드 샘플을 가중합)
			// else, 단일 샘플 처리
		}

		// 윈도우를 적용하는 신호 샘플
		for (int32_t i = _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels; i < _DJSW_WSOLA_FRAME_SIZE * _header->numChannels; ++i)
		{
			// if, 루프 경계에서 샘플 처리 (출력 버퍼에 바로 저장)
			// if, 크로스페이드 샘플 처리 (출력 버퍼의 값과 크로스페이드 샘플을 가중합)
			// else, 단일 샘플 처리
		}

		_shouldJump = false;
	}

	// 2. 템포 변경 로직
	if (_olaPosition >= _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels)
	{
		// hopDistance, tshDistance를 고려해 glbPosition을 갱신하며 _wsolaInputBuffer에 내용을 채워야 함.

		// 윈도우를 적용하는 신호 샘플
		for (int32_t i = 0; i < _DJSW_WSOLA_FRAME_SIZE * _header->numChannels; ++i)
		{
			// if, 루프 경계에서 샘플 처리 (_wsolaHanningBuffer에 저장)
			// if, 크로스페이드 샘플 처리 (_wsolaHanningBuffer의 값과 크로스페이드 샘플을 가중합)
			// else, 단일 샘플 처리
		}

		// _wsolaHanningBuffer에 Hann Window 적용

		for (int32_t i = 0; i < _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels; ++i)
		{
			_wsolaOutputBuffer[i] = _wsolaOutputBuffer[i + _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels] + _wsolaHanningBuffer[i];
			_wsolaOutputBuffer[i + _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels] = _wsolaOutputBuffer[i + _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels];
		}

		_olaPosition = 0;
	}

	// 3. 최종 샘플 출력
	for (int32_t i = 0; i < _header->numChannels; ++i)
	{
		out[i] = _wsolaOutputBuffer[_olaPosition++];
	}
}

void djAudioSource::Read(int16_t* out)
{
	
}