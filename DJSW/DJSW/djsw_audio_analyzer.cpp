#include "djsw_audio_analyzer.h"

static float GetAutoCorrelation(
	int16_t* a,
	int16_t* b, // b는 a를 시간축 평행이동한 배열임.
	int32_t sampleCount,
	int32_t samplesPerBar,
	int32_t compareLength)
{
	float sum_m = 0.0f;
	float sum_a = 0.0f;
	float sum_b = 0.0f;
	
	// Normalized Auto Correlation
	for (int32_t i = 0; i < sampleCount; i += samplesPerBar)
	{
		for (int32_t j = 0; j < compareLength; ++j)
		{
			float na = (float)a[i + j] / 32768.0f;
			float nb = (float)b[i + j] / 32768.0f;

			sum_m += (na * nb);
			sum_a += (na * na);
			sum_b += (nb * nb);
		}
	}

	float denominator = sqrtf(sum_a * sum_b);

	if (denominator == 0.0f)
		return 0.0f;

	return sum_m / denominator;
}

djAnalyzeJob::djAnalyzeJob()
	: djAnalyzeJob(NULL, 60.0f, 160.0f, 0.25f)
{

}

#pragma warning(disable: 26495)
djAnalyzeJob::djAnalyzeJob(
	char* wavFilePath,
	float bpmMin,
	float bpmMax,
	float bpmUnit) :
	_wavFilePath(wavFilePath),
	_bpmMin(bpmMin),
	_bpmMax(bpmMax),
	_bpmUnit(bpmUnit),
	_header(NULL)
{

}

bool djAnalyzeJob::Init()
{
	_bpm = _bpmMin;
	_offset = 0.0f;

	int32_t fileSize = 0;

	if (!LoadWavFile(_wavFilePath, &_header, &fileSize))
		return false;

	_samples = _header->samples;
	_sampleCount = _header->dataChunkSize / sizeof(int16_t);
	_sampleRate = _header->sampleRate;
	_numChannels = _header->numChannels;

	_optAutoCorrelation = -1.0f;
	_optBpm = _bpmMin;
	_optOffset = 0;

	_maxAutoCorrelation = -1.0f;
	_maxOffset = 0.0f;

	_samplesPerBar = GetSamplesPerBar(_sampleRate, _numChannels, _bpmMin);

	return true;
}

bool djAnalyzeJob::Analyze()
{
	float autoCorrelation = GetAutoCorrelation(
		_samples,
		_samples + (int32_t)((float)_samplesPerBar * (1.0f + _offset)),
		(_sampleCount - _samplesPerBar) / 2, // 전체 곡의 일부만 분석해봐도 BPM 파악이 가능함.
		_samplesPerBar,
		(int32_t)((float)_samplesPerBar * 0.25f) // 마디의 초반 부분이 큰 진폭 에너지를 갖는 경향이 있어 마디 초반만 분석해봐도 됨.
	);
	
	if (autoCorrelation > _maxAutoCorrelation)
	{
		_maxAutoCorrelation = autoCorrelation;
		_maxOffset = _offset;
	}

	_offset += 1 / 32.0f;

	if (_offset < 1.0f)
		return false;

	if (_maxAutoCorrelation > _optAutoCorrelation)
	{
		_optAutoCorrelation = _maxAutoCorrelation;
		_optBpm = _bpm;
		_optOffset = (int32_t)((float)_samplesPerBar * _maxOffset);
	}

	_bpm += _bpmUnit;
	_offset = 0.0f;

	_samplesPerBar = GetSamplesPerBar(_sampleRate, _numChannels, _bpm);
	_maxAutoCorrelation = -1.0f;
	_maxOffset = 0.0f;

	if (_bpm <= _bpmMax)
		return false;

	return true;
}

bool djAnalyzeJob::Final()
{
	result.bpm = _optBpm;
	result.firstBarIndex = _optOffset;
	result.barCount = -1;

	return true;
}

void djAnalyzeJob::Release()
{
	HeapFree(GetProcessHeap(), 0, _header);
}

int32_t GetSamplesPerBar(int32_t sampleRate, int32_t channelCount, float bpm)
{
	return (int32_t)((float)(sampleRate * channelCount * 240) / bpm);
}