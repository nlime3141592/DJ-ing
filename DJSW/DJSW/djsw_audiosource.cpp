#include "djsw_audiosource.h"

#include <assert.h>

#include "djsw_audio_analyzer.h"

djAudioSource::djAudioSource() :
	//_metaFile(new djWavMetaFile()),
	_metaFile(),
	_header(NULL),
	_audioFileSize(0),

	_numWavSamples(0),
	_wavSamples(NULL),

	_glbPosition(0),
	_olaPosition(0),

	_isPlaying(false),

	_useGlobalCue(false),
	_glbCueIndex(0),

	_shouldJump(false),
	_jumpIndex(0),

	_useLoop(false),
	_loopBarCount(0),
	_loopIndex(0),
	_loopLength(0),
	
	_hopDistance(0),
	_tshDistance(0),

	_wsolaInputSize(0),
	_wsolaInputBuffer0(NULL),
	_wsolaInputBuffer1(NULL),

	_wsolaHannBufferSize(0),
	_wsolaHannedValueBuffer(NULL),
	_wsolaHanningWindowBuffer(NULL),

	_wsolaOutputSize(0),
	_wsolaOutputBuffer(NULL),

	_wsolaSelectedTolerance(0),

	_xFadeBeg(0),
	_xFadeSampleLeft(0)
{
	
}

djAudioSource::~djAudioSource()
{
	//delete _metaFile;
}

bool djAudioSource::IsLoop()
{
	return _useLoop;
}

bool djAudioSource::IsPlaying()
{
	return _isPlaying;
}

int32_t djAudioSource::GetPosition()
{
	return _glbPosition + _olaPosition;
}

wstring djAudioSource::CreateMetadata(wstring wavPath)
{
	wstring metaFilePath = wavPath + L".djmeta";

	_metaFile.Open(metaFilePath);
	_metaFile.Init();
	_metaFile.SetWavFile(wavPath);
	_metaFile.Close();

	return metaFilePath;
}

void djAudioSource::Play()
{
	_olaPosition = 0;
	_wsolaSelectedTolerance = 0;
	ReadInit();

	_useGlobalCue = false;
	_isPlaying = true;
}

void djAudioSource::Pause()
{
	_isPlaying = false;

	_glbPosition += _olaPosition;
	_olaPosition = 0;
}

void djAudioSource::PlayGlobalCue()
{
	_olaPosition = 0;
	_wsolaSelectedTolerance = 0;
	ReadInit();

	_useGlobalCue = true;
}

void djAudioSource::PauseGlobalCue()
{
	_useGlobalCue = false;

	_glbPosition += _olaPosition;
	_olaPosition = 0;
}

char* djAudioSource::GetWavFilePath()
{
	return _metaFile.GetWavFilePath();
}

bool djAudioSource::IsLoaded()
{
	return _header != NULL;
}

bool djAudioSource::Load(wstring metaFilePath)
{
	// 1. Load Meta File
	if (!_metaFile.Open(metaFilePath))
		return false;

	// 2. Load Audio File
	if (!LoadWavFile(_metaFile.GetWavFilePath(), &_header, &_audioFileSize))
		return false;

	assert(IsValidWavFile(_header));

	// 3. Post processing.
	_numWavSamples = _header->dataChunkSize / DJSW_BYTES_PER_SAMPLE;
	_wavSamples = _header->samples;

	// TODO: 일반적인 경우, 전체 배열을 쓰려면 HeapAlloc으로 할당한 바이트 수와 memcpy에 쓸 바이트 수가 같아야 하는데, 나는 계산상 차이가 있는지 2배 차이나게 해야 동작하는 상황이다. 점검 필요.
	_wsolaInputSize = _header->numChannels * (_DJSW_WSOLA_FRAME_SIZE + 2 * _DJSW_WSOLA_MAX_TOLERANCE);
	_wsolaInputBuffer0 = (int16_t*)HeapAlloc(GetProcessHeap(), 0, _wsolaInputSize * sizeof(int16_t));
	_wsolaInputBuffer1 = (int16_t*)HeapAlloc(GetProcessHeap(), 0, _wsolaInputSize * sizeof(int16_t));

	_wsolaHannBufferSize = _header->numChannels * _DJSW_WSOLA_FRAME_SIZE;
	_wsolaHannedValueBuffer = (int16_t*)HeapAlloc(GetProcessHeap(), 0, _wsolaHannBufferSize * sizeof(int16_t));
	_wsolaHanningWindowBuffer = (float*)HeapAlloc(GetProcessHeap(), 0, _wsolaHannBufferSize * sizeof(float));

	for (int32_t i = 0; i < _DJSW_WSOLA_FRAME_SIZE; ++i)
	{
		float hannValue = 0.5f - 0.5f * cosf(2.0f * 3.14159265358979323846f * (float)i / (float)(_DJSW_WSOLA_FRAME_SIZE - 1));

		for (int32_t j = 0; j < _header->numChannels; ++j)
		{
#pragma warning(disable: 6011)
			_wsolaHanningWindowBuffer[i * _header->numChannels + j] = hannValue;
		}
	}

	_wsolaOutputSize = _header->numChannels * _DJSW_WSOLA_FRAME_SIZE;
	_wsolaOutputBuffer = (int16_t*)HeapAlloc(GetProcessHeap(), 0, _wsolaOutputSize * sizeof(int16_t));

	return true;
}

bool djAudioSource::Unload()
{
	if (!IsLoaded())
		return false;

	_metaFile.Save();
	_metaFile.Close();
	
	HeapFree(GetProcessHeap(), 0, _header);
	_header = NULL;
	_audioFileSize = 0;

	_numWavSamples = 0;
	_wavSamples = NULL;

	_glbPosition = 0;
	_olaPosition = 0;

	_isPlaying = false;

	_useGlobalCue = false;
	_glbCueIndex = 0;

	_shouldJump = false;
	_jumpIndex = 0;

	_useLoop = false;
	_loopIndex = 0;
	_loopLength = 0;

	_hopDistance = 0;
	_tshDistance = 0;

	_wsolaInputSize = 0;
	HeapFree(GetProcessHeap(), 0, _wsolaInputBuffer0);
	HeapFree(GetProcessHeap(), 0, _wsolaInputBuffer1);
	_wsolaInputBuffer0 = NULL;

	HeapFree(GetProcessHeap(), 0, _wsolaHannedValueBuffer);
	HeapFree(GetProcessHeap(), 0, _wsolaHanningWindowBuffer);
	_wsolaHannedValueBuffer = NULL;
	_wsolaHanningWindowBuffer = NULL;

	_wsolaOutputSize = 0;
	HeapFree(GetProcessHeap(), 0, _wsolaOutputBuffer);
	_wsolaOutputBuffer = NULL;

	_wsolaSelectedTolerance = 0;

	_xFadeBeg = 0;
	_xFadeSampleLeft = 0;

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

void djAudioSource::Jump(int32_t jumpIndex)
{
	_jumpIndex = jumpIndex;
	_shouldJump = true;
}

void djAudioSource::SetLoop(int32_t loopBarCount, bool shouldQuantize)
{
	//int32_t samplesPerBar = GetSamplesPerBar(_header->sampleRate, _header->numChannels, _metaFile.GetBpm());
	int32_t samplesPerBar = GetSamplesPerBar(_header->sampleRate, _header->numChannels, 126.0f); // TEST BPM.

	if (loopBarCount == DJSW_BAR_COUNT_0 || loopBarCount == _loopBarCount)
	{
		_useLoop = false;
		_loopBarCount = 0;
		_loopLength = 0;
		_loopIndex = -1;
		return;
	}
	if (loopBarCount > 0)
	{
		_loopBarCount = loopBarCount;
		_loopLength = samplesPerBar * loopBarCount;
		_useLoop = true;
	}
	if (loopBarCount < 0)
	{
		_loopBarCount = loopBarCount;
		_loopLength = samplesPerBar / (-loopBarCount);
		_useLoop = true;
	}
	
	int32_t position = _glbPosition + _olaPosition;

	if (shouldQuantize)
	{
		int32_t idxLeft = _metaFile.GetFirstBarIndex() + position / samplesPerBar;
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

void djAudioSource::SetTimeShiftDistance(int32_t timeShiftSamples)
{
	_tshDistance = timeShiftSamples * _header->numChannels;
}

void djAudioSource::ClearHotCue(int hotCueIndex)
{
	_metaFile.SetHotCue(hotCueIndex, -1);
}

void djAudioSource::SetHotCue(int hotCueIndex)
{
	_metaFile.SetHotCue(hotCueIndex, _glbPosition + _olaPosition);
}

int32_t djAudioSource::GetHotCue(int hotCueIndex)
{
	int32_t position = _metaFile.GetHotCue(hotCueIndex);
	return position;
}

void djAudioSource::ReadInit()
{
	_glbPosition = LoadInputBuffer(_wsolaInputBuffer0, _glbPosition);

	memcpy(_wsolaOutputBuffer, _wsolaInputBuffer0 + _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels, _wsolaOutputSize);
	ApplyHanningWindow(_wsolaOutputBuffer, _wsolaHannBufferSize / 2, _wsolaHannBufferSize / 2);
}

void djAudioSource::Read(int16_t* out)
{
	// 1. 즉시 점프 로직 (Global/Hot Cue)
	if (_shouldJump)
	{
		_xFadeBeg = _glbPosition + _olaPosition;
		_xFadeSampleLeft = DJSW_CROSSFADE_SAMPLE_LENGTH;

		_glbPosition = _jumpIndex;
		_olaPosition = 0;
		
		ReadInit();

		_shouldJump = false;
	}

	// 2. 시간축 스케일링 로직
	if (_olaPosition >= _wsolaOutputSize / 2)
	{
		int32_t hop = (_hopDistance + _tshDistance) * _header->numChannels;

		if (_useLoop)
			hop %= _loopLength;

		// 입력 버퍼를 적절히 수정해야 함.
		_glbPosition += (_wsolaOutputSize / 2) + hop;
		_tshDistance = 0;
		_olaPosition = 0;
		_glbPosition = LoadInputBuffer(_wsolaInputBuffer1, _glbPosition);

		// 위상 보정 offset을 얻어냄.
		int32_t tolerance = (int32_t)(0.2f * (float)hop);

		if (tolerance < -_DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels)
			tolerance = -_DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels;
		if (tolerance > _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels)
			tolerance = _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels;

		int32_t offset = SeekBestOverlapPosition(tolerance);
		
		// _wsolaHannedValueBuffer에 Hann Window 적용
		memcpy(_wsolaHannedValueBuffer, _wsolaInputBuffer1 + _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels + offset, _wsolaHannBufferSize * sizeof(int16_t));

		// 윈도우를 적용하는 신호 샘플
		// TODO: 왜 Window 함수만 적용하면 소리가 이상해지는지 이유 점검.
		ApplyHanningWindow(_wsolaHannedValueBuffer, 0, _wsolaHannBufferSize);

		for (int32_t i = 0; i < _wsolaOutputSize / 2; ++i)
		{
			_wsolaOutputBuffer[i] = _wsolaOutputBuffer[i + _wsolaOutputSize / 2] + _wsolaHannedValueBuffer[i];
			_wsolaOutputBuffer[i + _wsolaOutputSize / 2] = _wsolaHannedValueBuffer[i + _wsolaOutputSize / 2];
		}

		memcpy(_wsolaInputBuffer0, _wsolaInputBuffer1, _wsolaInputSize);
		_wsolaSelectedTolerance = offset;
	}

	// 3. 최종 샘플 출력
	// NOTE: 조건을 for문 안에서 검사하면 출력 도중 _isPlaying이 변경되는 경우, 샘플의 원자성이 깨짐.
	// TODO: 코드 사이즈를 줄일 수 있을까?
	if (_isPlaying || _useGlobalCue)
	{
		int32_t position = _glbPosition + _olaPosition;

		if (position < 0)
		{
			for (int32_t i = 0; i < _header->numChannels; ++i)
			{
				out[i] = 0.0f;
				++_olaPosition;
			}
		}
		else if (position < _numWavSamples)
		{
			for (int32_t i = 0; i < _header->numChannels; ++i)
			{
				out[i] = _wsolaOutputBuffer[_olaPosition++];
			}
		}
		else
		{
			_isPlaying = false;
			_useGlobalCue = false;

			for (int32_t i = 0; i < _header->numChannels; ++i)
			{
				out[i] = 0.0f;
			}
		}
	}
	else
	{
		if (_glbPosition + _tshDistance >= _numWavSamples)
		{
			_glbPosition = _numWavSamples;
		}
		else
		{
			_glbPosition += _tshDistance;
			_tshDistance = 0;
		}

		for (int32_t i = 0; i < _header->numChannels; ++i)
		{
			out[i] = 0.0f;
		}
	}
}

void djAudioSource::Peek(int32_t scale, int32_t offset, int32_t idxChannel, int16_t* outMin, int16_t* outMax)
{
	assert(scale > 0);
	assert(idxChannel >= 0 && idxChannel < _header->numChannels);

	int16_t min = 0x7FFF;
	int16_t max = 0x8000;

	// NOTE: 괄호 및 연산 순서 주의.
	int32_t position = scale * ((_glbPosition + _olaPosition + offset * _header->numChannels) / scale);
	int32_t beg = position;
	int32_t end = position + scale * _header->numChannels;

	if (beg < 0)
		beg = 0;
	if (end >= _numWavSamples - _header->numChannels)
		end = _numWavSamples - _header->numChannels;
	
	if (end <= beg)
	{
		*outMin = 0;
		*outMax = 0;
		return;
	}

	for (int32_t i = beg; i <= end; i += _header->numChannels)
	{
		int16_t sample = _wavSamples[i + idxChannel];

		if (sample < min)
			min = sample;
		if (sample > max)
			max = sample;
	}

	*outMin = min;
	*outMax = max;
}

void djAudioSource::SetHopDistance(int32_t hopDistance)
{
	_hopDistance = hopDistance;
}

int16_t djAudioSource::GetNumChannels()
{
	return _header->numChannels;
}

int32_t djAudioSource::LoadInputBuffer(int16_t* inputBuffer, int32_t originPosition)
{
	int32_t i = originPosition - _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels;
	int32_t j = i;
	int32_t k = 0;
	
	while (k < _wsolaInputSize)
	{
		j = i;

		// 루프 경계에서 샘플 처리
		if (_useLoop)
		{
			j = (i - _loopIndex) % _loopLength;

			if (j < 0)
				j += _loopLength;
			if (j == 0)
			{
				_xFadeBeg = _loopIndex + _loopLength;
				_xFadeSampleLeft = DJSW_CROSSFADE_SAMPLE_LENGTH * _header->numChannels;
			}

			j += _loopIndex;
		}

		// 단일 샘플 처리
		if (j >= 0 && j < _numWavSamples)
			inputBuffer[k] = _wavSamples[j];
		else
			inputBuffer[k] = 0.0f;

		// 크로스페이드 샘플 처리
		if (_xFadeSampleLeft > 0)
		{
			float w0 = (float)_xFadeSampleLeft / (DJSW_CROSSFADE_SAMPLE_LENGTH * _header->numChannels);
			float w1 = 1.0f - w0;

			int32_t d0 = DJSW_CROSSFADE_SAMPLE_LENGTH - _xFadeSampleLeft;
			int32_t d1 = _xFadeSampleLeft;

			int32_t i0 = _xFadeBeg + d0;
			int32_t i1 = j - d1;

			int16_t s0 = 0;
			int16_t s1 = 0;

			if (i0 >= 0 && i0 < _numWavSamples)
				s0 = _wavSamples[i0];
			if (i1 >= 0 && i1 < _numWavSamples)
				s1 = _wavSamples[i1];

			inputBuffer[k] = w0 * s0 + w1 * s1;
			//OutputDebugStringW((L"2 - k == " + to_wstring(k) + L", hotCueIndices[0] == " + to_wstring(_metaFile.GetHotCue(0)) + L"\n").c_str());

			--_xFadeSampleLeft;
		}

		++i;
		++k;
	}

	// next global position.
	if (_useLoop)
		originPosition = _loopIndex + (originPosition - _loopIndex) % _loopLength;

	if (originPosition < _numWavSamples)
		return originPosition;
	else
		return _numWavSamples;
}

void djAudioSource::ApplyHanningWindow(int16_t* buffer, int32_t begin, int32_t length)
{
	for (int32_t i = 0; i < length; ++i)
	{
		float value = (float)buffer[begin + i] * _wsolaHanningWindowBuffer[begin + i];
		buffer[begin + i] = (int16_t)value;
	}
}

float djAudioSource::GetCrossCorrelation(int16_t* a, int16_t* b, int32_t length)
{
	float sum_a = 0.0f;
	float sum_b = 0.0f;
	float sum_m = 0.0f;

	for (int32_t i = 0; i < _DJSW_WSOLA_FRAME_SIZE; ++i)
	{
		float v0 = (float)a[i] / 32768.0f;
		float v1 = (float)b[i] / 32768.0f;

		sum_a += (v0 * v0);
		sum_b += (v1 * v1);
		sum_m += (v0 * v1);
	}

	if (sum_a == 0.0f || sum_b == 0.0f)
		return 0.0f;

	return sum_m / sqrtf(sum_a * sum_b);
}

int32_t djAudioSource::SeekBestOverlapPosition(int32_t tolerance)
{
	float maxCorr = FLT_MIN;
	int32_t maxOffset = 0;

	int32_t cT = _DJSW_WSOLA_MAX_TOLERANCE * _header->numChannels;
	int32_t cO = _DJSW_WSOLA_OVERLAP_SIZE * _header->numChannels;

	int16_t* prevBuffer = _wsolaInputBuffer0 + cT + _wsolaSelectedTolerance + cO;
	int16_t* nextBuffer = _wsolaInputBuffer1 + cT;

	for (int32_t i = -tolerance * _header->numChannels; i <= tolerance * _header->numChannels; i += _header->numChannels)
	{
		float corr = GetCrossCorrelation(prevBuffer, nextBuffer + i, cO);
		
		if (corr > maxCorr)
		{
			maxCorr = corr;
			maxOffset = i;
		}
	}

	return maxOffset;
}