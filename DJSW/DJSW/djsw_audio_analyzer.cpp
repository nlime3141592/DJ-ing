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

int32_t GetSamplesPerBar(int32_t sampleRate, int32_t channelCount, float bpm)
{
	return (int32_t)((float)(sampleRate * channelCount * 240) / bpm);
}

void AnalyzeGridData(
	djAudioAnalyzerParams* params,
	djWavGridData* resultBuffer)
{
	float optAutoCorrelation = -1.0f;
	float optBpm = params->bpmMin;
	int32_t optOffset = 0.0f;

	for (float bpm = params->bpmMin; bpm <= params->bpmMax; bpm += params->bpmUnit)
	{
		int32_t samplesPerBar = GetSamplesPerBar(params->sampleRate, params->channelCount, bpm);

		float maxAutoCorrelation = -1.0f;
		float maxOffset = 0.0f;
		
		for (float offset = 0.0f; offset < 1.0f; offset += 1 / 32.0f)
		{
			float autoCorrelation = GetAutoCorrelation(
				params->samples,
				params->samples + (int32_t)((float)samplesPerBar * (1.0f + offset)),
				(params->sampleCount - samplesPerBar) / 2, // 전체 곡의 일부만 분석해봐도 BPM 파악이 가능함.
				samplesPerBar,
				(int32_t)((float)samplesPerBar * 0.25f) // 마디의 초반 부분이 큰 진폭 에너지를 갖는 경향이 있어 마디 초반만 분석해봐도 됨.
			);

			if (autoCorrelation > maxAutoCorrelation)
			{
				maxAutoCorrelation = autoCorrelation;
				maxOffset = offset;
			}
		}
		
		if (maxAutoCorrelation > optAutoCorrelation)
		{
			optAutoCorrelation = maxAutoCorrelation;
			optBpm = bpm;
			optOffset = (int32_t)((float)samplesPerBar * maxOffset);
		}
	}

	resultBuffer->bpm = optBpm;
	resultBuffer->firstBarIndex = optOffset;
	resultBuffer->barCount = -1;
}