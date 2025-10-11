#include "audioutil.h"

static constexpr int MAX_ARRAY_SIZE = 1024;
static constexpr int MAX_TIMING_SIZE = 1048576;

static float cArray[MAX_ARRAY_SIZE]; // 자기상관계수 배열
static float tArray[MAX_ARRAY_SIZE]; // tau-array
static int oArray[MAX_ARRAY_SIZE]; // 타이밍 오프셋 배열

static float sampleSum[MAX_TIMING_SIZE];

static constexpr float AccentFilterCoef1(float w0, float w1)
{
	return (64.0f * w0 - 36.0f * w1 - 28.0f) / 3.0f;
}

static constexpr float AccentFilterCoef2(float w0, float w1)
{
	return (-320.0f * w0 + 228.0f * w1 + 92.0f) / 3.0f;
}

static constexpr float AccentFilterCoef3(float w0, float w1)
{
	return (512.0f * w0 - 384.0f * w1 - 128.0f) / 3.0f;
}

static constexpr float AccentFilterCoef4(float w0, float w1)
{
	return (-256.0f * w0 + 192.0f * w1 + 64.0f) / 3.0f;
}

static float AccentFilter(float tau, float* buffer, int offset, int length, int channel)
{
	float sum = 0.0f;
	float w0 = 0.3f;
	float w1 = 0.6f;

	int i0 = length / 2;

	for (int i = 0; i < length / 2; i += channel)
	{
		float x = (float)((i + offset) / length);
		float y = 0.0f;
		float f = 1.0f;

		y += (f *= x) * AccentFilterCoef1(w0, w1);
		y += (f *= x) * AccentFilterCoef2(w0, w1);
		y += (f *= x) * AccentFilterCoef3(w0, w1);
		y += (f *= x) * AccentFilterCoef4(w0, w1);

		for (int j = 0; j < channel; ++j)
		{
			float sample = buffer[i + j] + buffer[length - channel - i + j];
			sum += y * sample;
		}
	}

	return 1.0f + sum / length;
}

float GetAutoCorrelation(
	AutoCorrelation* output,
	float bpmMin,
	float bpmMax,
	float bpmDelta,
	int16_t* buffer,
	int offset,
	int length,
	int fs,
	int channel)
{
	int count = 0;
	float bpm = bpmMin;
	
	for (; bpm < bpmMax; bpm += bpmDelta)
	{
		// channel 변수가 있어야만 한다. 없다면 아래와 같은 현상이 있다.
		// 1채널 샘플이라면 1마디 간격
		// 2채널 샘플이라면 1/2마디 간격 (샘플이 l r l r 순으로 버퍼에 배치되어 있기 때문임)
		tArray[count++] = 480.0f * channel * fs / bpm;
	}

	for (int i = 0; i < count; ++i)
	{
		cArray[i] = 0.0f;

		float tf = tArray[i];
		int ti = (int)tf;
		printf("%d/%d\n", i, count);
		for (int j = 0; j < ti; ++j)
		{
			sampleSum[j] = 0.0f;
		}

		for (int j = 0; j < length; ++j)
		{
			if (j >= length / 4 && j < 3 * length / 4)
				sampleSum[j % ti] += buffer[j];

			int k = j + tArray[i];

			float x0 = (float)buffer[j] / fs;
			float x1 = 0.0f;

			if (k < length)
				x1 = (float)buffer[k] / fs;

			cArray[i] += (x0 * x1);
		}

		int iMaxCof = -1;
		int cMaxCof = 0.0f;
		int dti = ti / 512;

		for (int j = 0; j < ti ; j += dti)
		{
			//float cof = AccentFilter(tf, sampleSum, j, ti, channel);
			float cof = sampleSum[j];

			if (cof < 0.0f) cof *= -1.0f;

			//printf("%d/%d\n", j, ti);

			if (cof > cMaxCof)
			{
				iMaxCof = j;
				cMaxCof = cof;
			}
		}

		oArray[i] = iMaxCof;
	}

	int iMax = -1;
	float cMax = 0.0f;

	for (int i = 0; i < count; ++i)
	{
		if (cArray[i] > cMax)
		{
			iMax = i;
			cMax = cArray[i];
		}
	}

	output->bpm = bpmMin + iMax * bpmDelta;
	output->offset = oArray[iMax];
	output->tau = tArray[iMax];

	return 0.0f;

	// 제대로 동작하는 자기상관함수.
	/*for (int i = 0; i < length; ++i)
	{
		for (int j = 0; j < count; ++j)
		{
			int k = i + tArray[j];

			float x0 = (float)buffer[i] / fs;
			float x1 = 0.0f;

			if (k < length)
				x1 = (float)buffer[k] / fs;

			cArray[j] += (x0 * x1);
		}
	}

	int iMax = -1;
	float cMax = 0.0f;

	for (int i = 0; i < count; ++i)
	{
		if (cArray[i] > cMax)
		{
			iMax = i;
			cMax = cArray[i];
		}
	}
	
	return bpmMin + iMax * bpmDelta;*/
}