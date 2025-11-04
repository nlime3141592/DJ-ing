#include "LoopAudio.h"

AudioDevice _audioDevice;
AudioChannel _channel0;
AudioChannel _channel1;

static inline void MixSample1(const int16_t* s0, const int16_t* s1, int16_t* out)
{
	float fSample0 = (float)*s0;
	float fSample1 = (float)*s1;

	float mix = (fSample0 + fSample1) / 32768.0f;

	// hyper tangent approximation (clipping)
	// tanh(x) = x * (27 + x^2) / (27 + 9x^2)
	float x2 = mix * mix;
	float t = mix * (27.0f + x2) / (27.0f + 9.0f * x2);

	*out = (int16_t)(32767.0f * t);
}

static inline void MixSample16(const int16_t* s0, const int16_t* s1, int16_t* out)
{
	// Load 16-bit data into 256-bit register
	__m256i sample0 = _mm256_loadu_si256((const __m256i*)s0);
	__m256i sample1 = _mm256_loadu_si256((const __m256i*)s1);

	// convert 16-bit to 32-bit integer
	__m256i sample0_lo = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(sample0));
	__m256i sample0_hi = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(sample0, 1));
	__m256i sample1_lo = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(sample1));
	__m256i sample1_hi = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(sample1, 1));

	// convert 32-bit integer to 32-bit float
	__m256 fSample0_lo = _mm256_cvtepi32_ps(sample0_lo);
	__m256 fSample0_hi = _mm256_cvtepi32_ps(sample0_hi);
	__m256 fSample1_lo = _mm256_cvtepi32_ps(sample1_lo);
	__m256 fSample1_hi = _mm256_cvtepi32_ps(sample1_hi);

	// normalization
	const __m256 norm = _mm256_set1_ps(1.0f / 32768.0f);
	fSample0_lo = _mm256_mul_ps(fSample0_lo, norm);
	fSample0_hi = _mm256_mul_ps(fSample0_hi, norm);
	fSample1_lo = _mm256_mul_ps(fSample1_lo, norm);
	fSample1_hi = _mm256_mul_ps(fSample1_hi, norm);

	// mix
	__m256 mix_lo = _mm256_add_ps(fSample0_lo, fSample1_lo);
	__m256 mix_hi = _mm256_add_ps(fSample0_hi, fSample1_hi);

	// hyper tangent approximation (clipping)
	// tanh(x) = x * (27 + x^2) / (27 + 9x^2)
	const __m256 c27 = _mm256_set1_ps(27.0f);
	const __m256 c9 = _mm256_set1_ps(9.0f);

	__m256 x2_lo = _mm256_mul_ps(mix_lo, mix_lo);
	__m256 x2_hi = _mm256_mul_ps(mix_hi, mix_hi);

	__m256 t_lo = _mm256_div_ps(_mm256_mul_ps(mix_lo, _mm256_add_ps(c27, x2_lo)), _mm256_add_ps(c27, _mm256_mul_ps(c9, x2_lo)));
	__m256 t_hi = _mm256_div_ps(_mm256_mul_ps(mix_hi, _mm256_add_ps(c27, x2_hi)), _mm256_add_ps(c27, _mm256_mul_ps(c9, x2_hi)));

	// 32-bit float to 16-bit integer
	const __m256 scale = _mm256_set1_ps(32767.0f);
	t_lo = _mm256_mul_ps(t_lo, scale);
	t_hi = _mm256_mul_ps(t_hi, scale);

	__m256i i32_lo = _mm256_cvtps_epi32(t_lo);
	__m256i i32_hi = _mm256_cvtps_epi32(t_hi);

	__m128i i16_lo = _mm256_cvtepi32_epi16(i32_lo);
	__m128i i16_hi = _mm256_cvtepi32_epi16(i32_hi);

	__m256i packed = _mm256_set_m128i(i16_hi, i16_lo);

	_mm256_storeu_si256((__m256i*)out, packed);
}

void AudioInit()
{
	_audioDevice = AudioDevice();
	_audioDevice.Init();
	_audioDevice.Start();

	_channel0 = AudioChannel();
	_channel1 = AudioChannel();

	// TEST: for Debugging.
	//bool result = _channel0.Load("C:\\Test\\habibi.wav");
	//assert(result);
	//_channel0.Play();
}

void AudioLoop()
{
	// 1. 버퍼 크기 얻기
	UINT32 bufferSizeInFrames;
	HRESULT hr = _audioDevice.audioClient->GetBufferSize(&bufferSizeInFrames);
	assert(hr == S_OK);

	// 2. 패딩 버퍼 확인
	UINT32 bufferPadding;
	hr = _audioDevice.audioClient->GetCurrentPadding(&bufferPadding);
	assert(hr == S_OK);

	// 3. 목표 버퍼 패딩 정의
	const float TARGET_BUFFER_PADDING_IN_SECONDS = 1 / 60.0f;
	UINT32 targetBufferPadding = UINT32(bufferSizeInFrames * TARGET_BUFFER_PADDING_IN_SECONDS);
	UINT32 numFramesToWrite = targetBufferPadding - bufferPadding;

	// 4. 렌더 버퍼 얻기
	int16_t* buffer;
	hr = _audioDevice.audioRenderClient->GetBuffer(numFramesToWrite, (BYTE**)(&buffer));
	assert(hr == S_OK);

	// 5. 샘플 쓰기
	UINT32 idx = 0;

	// 4-channel SIMD Processing (AVX2 Instruction Set Needs.)
	for (; idx < numFramesToWrite; idx += 1)
	{
		// TODO: 매 샘플마다 입력 처리가 필요한가?
		float samples[8] = { 0.0f };
	}

	// 6. 샘플 제출
	hr = _audioDevice.audioRenderClient->ReleaseBuffer(numFramesToWrite, 0);
	assert(hr == S_OK);


	// 7. 재생 타이밍 정보 확인 (선택사항)
	IAudioClock* audioClock;
	_audioDevice.audioClient->GetService(__uuidof(IAudioClock), (LPVOID*)(&audioClock));
	UINT64 audioPlaybackFreq;
	UINT64 audioPlaybackPos;
	audioClock->GetFrequency(&audioPlaybackFreq);
	audioClock->GetPosition(&audioPlaybackPos, 0);
	audioClock->Release();
	// UINT64 audioPlaybackPosInSeconds = audioPlaybackPos/audioPlaybackFreq;
	// UINT64 audioPlaybackPosInSamples = audioPlaybackPosInSeconds*mixFormat.nSamplesPerSec;
}

void AudioFinal()
{
	_audioDevice.audioClient->Stop();
	_audioDevice.audioClient->Release();
	_audioDevice.audioRenderClient->Release();

	_channel0.Unload();
	_channel1.Unload();
}