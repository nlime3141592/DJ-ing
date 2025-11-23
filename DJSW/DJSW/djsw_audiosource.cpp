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
		_glbPosition = _jumpIndex - _olaPosition;
		LoadInputBuffer(0);

		// 윈도우를 적용하지 않는 신호 샘플
		for (int32_t i = _olaPosition * _header->numChannels; i < _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels; ++i)
		{
			_wsolaOutputBuffer[i] = _wsolaInputBuffer[i + _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels];
		}

		memcpy(_wsolaHannedValueBuffer, _wsolaInputBuffer + _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels, _DJSW_WSOLA_FRAME_SIZE * _header->numChannels);
		ApplyHanningWindow(_wsolaHannedValueBuffer, _DJSW_WSOLA_FRAME_SIZE * _header->numChannels);

		// 윈도우를 적용하는 신호 샘플
		for (int32_t i = _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels; i < _DJSW_WSOLA_FRAME_SIZE * _header->numChannels; ++i)
		{
			_wsolaOutputBuffer[i] = _wsolaHannedValueBuffer[i];
		}

		_wsolaSelectedTolerance = 0;
		_shouldJump = false;
	}

	// 2. 템포 변경 로직
	if (_olaPosition >= _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels)
	{
		int32_t hop = _hopDistance + _tshDistance;

		if (_useLoop)
			hop %= _loopLength;

		// 입력 버퍼를 적절히 수정해야 함.
		_glbPosition += _DJSW_WSOLA_OVERLAP_SIZE + hop;
		_tshDistance = 0;
		_olaPosition = 0;
		LoadInputBuffer(hop);

		// 윈도우를 적용하는 신호 샘플
		int32_t tolerance = (int32_t)(0.2f * (float)hop);

		if (tolerance < -_DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels)
			tolerance = -_DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels;
		if (tolerance > _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels)
			tolerance = _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels;

		// 위상 보정 offset을 얻어냄
		int32_t offset = SeekBestOverlapPosition(tolerance);
		
		// _wsolaHanningBuffer에 Hann Window 적용
		memcpy(_wsolaHannedValueBuffer, _wsolaInputBuffer + _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels + offset, _DJSW_WSOLA_FRAME_SIZE * _header->numChannels);
		ApplyHanningWindow(_wsolaHannedValueBuffer, _DJSW_WSOLA_FRAME_SIZE * _header->numChannels);

		for (int32_t i = 0; i < _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels; ++i)
		{
			_wsolaOutputBuffer[i] = _wsolaOutputBuffer[i + _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels] + _wsolaHannedValueBuffer[i];
			_wsolaOutputBuffer[i + _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels] = _wsolaOutputBuffer[i + _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels];
		}

		_wsolaSelectedTolerance = offset;
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

void djAudioSource::LoadInputBuffer(int32_t hop)
{
	// hopDistance, tshDistance를 고려해 glbPosition을 갱신하며 _wsolaInputBuffer에 내용을 채워야 함.
		// 입력 버퍼를 적절히 수정해야 함.
	int32_t inputLength = _header->numChannels * (_DJSW_WSOLA_FRAME_SIZE + 2 * _DJSW_WSOLA_MAX_TOLERANCE);
	int32_t i = _glbPosition - _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels;
	int32_t k = 0;
	
	while (k < inputLength)
	{
		int32_t j = i;

		// 루프 경계에서 샘플 처리
		if (_useLoop)
		{
			j = (i - _loopIndex) % _loopLength;

			if (j < 0)
				j += _loopLength;
			if (j == 0)
			{
				_xFadeBeg = _loopIndex + _loopLength;
				_xFadeSampleLeft = _xFadeSampleLength;
			}

			j += _loopIndex;
		}

		// 단일 샘플 처리
		if (j < 0 || j >= _numWavSamples)
			_wsolaInputBuffer[k] = _wavSamples[j];

		// 크로스페이드 샘플 처리
		if (_xFadeSampleLeft > 0)
		{
			float w0 = (float)_xFadeSampleLeft / _xFadeSampleLength;
			float w1 = 1.0f - w0;

			int32_t d0 = _xFadeSampleLength - _xFadeSampleLeft;
			int32_t d1 = _xFadeSampleLeft;

			int32_t i0 = _xFadeBeg + d0;
			int32_t i1 = j - d1;

			int16_t s0 = 0;
			int16_t s1 = 0;

			if (i0 >= 0 && i0 < _numWavSamples)
				s0 = _wavSamples[i0];
			if (i1 >= 0 && i1 < _numWavSamples)
				s1 = _wavSamples[i1];

			_wsolaInputBuffer[k] = w0 * s0 + w1 * s1;

			--_xFadeSampleLeft;
		}

		_olaPosition = 0;
		++i;
		++k;
	}
}

void djAudioSource::ApplyHanningWindow(int16_t* buffer, int32_t length)
{
	for (int i = 0; i < length; ++i)
	{
		buffer[i] *= _wsolaHanningWindowBuffer[i];
	}
}

float djAudioSource::GetCrossCorrelation(int16_t* a, int16_t* b)
{
	float sum_a = 0.0f;
	float sum_b = 0.0f;
	float sum_m = 0.0f;

	int16_t a0[_DJSW_WSOLA_FRAME_SIZE];
	int16_t b0[_DJSW_WSOLA_FRAME_SIZE];

	memcpy(a0, a, _DJSW_WSOLA_FRAME_SIZE);
	memcpy(b0, b, _DJSW_WSOLA_FRAME_SIZE);

	for (int32_t i = 0; i < _DJSW_WSOLA_FRAME_SIZE; ++i)
	{
		float v0 = (float)a0[i] / 32768.0f;
		float v1 = (float)b0[i] / 32768.0f;

		sum_a += (v0 * v0);
		sum_b += (v1 * v1);
		sum_m += (v0 * v1);
	}

	return sum_m / sqrtf(sum_a * sum_b);
}

int32_t djAudioSource::SeekBestOverlapPosition(int32_t tolerance)
{
	float maxCorr = FLT_MIN;
	int32_t maxOffset = 0;

	int16_t* prevBuffer = _wavSamples + _wsolaSelectedTolerance;
	int16_t* nextBuffer = _wavSamples + _glbPosition;

	for (int32_t i = -tolerance * _header->numChannels; i <= tolerance * _header->numChannels; i += _header->numChannels)
	{
		float corr = GetCrossCorrelation(prevBuffer, nextBuffer + i);

		if (corr > maxCorr)
		{
			maxCorr = corr;
			maxOffset = i;
		}
	}

	return maxOffset;
}