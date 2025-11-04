#pragma once
#pragma once

#include <immintrin.h>
#include <math.h>
#include <stdint.h>

#define PI_F 3.14159265358979323846f
#define DEG2RAD_F 0.01745329251994329577f
#define RAD2DEG_F 57.2957795130823208768f
#define INV_SQRT_2 0.7071067811865475244f

#define INDEX_Q 0
#define INDEX_F 1
#define INDEX_FS 2
#define INDEX_GAIN 3

// biquad 필터 구조(2차 IIR 필터)
struct BiquadFilterBuffer
{
	// WARNING: 순서가 바뀌면 안 됩니다.
	// parameters.
	float q;
	float f;
	float fs;
	float gain;

	// coefficients.
	float a0;
	float a1;
	float a2;
	float b0;
	float b1;
	float b2;

	// for data convolution.
	float y1;
	float y2;
	float x1;
	float x2;
};

// Stereo Pair 4개를 동시에 처리하기 위한 데이터 구조(2채널 * 4 Pair = 8개)
struct BiquadFilterRecordSIMD
{
	float data[8];
};

// biquad 필터 구조(2차 IIR 필터) - SIMD 방식
struct BiquadFilterBufferSIMD
{
	BiquadFilterRecordSIMD q;
	BiquadFilterRecordSIMD f;
	BiquadFilterRecordSIMD fs;
	BiquadFilterRecordSIMD gain;

	// coefficients.
	BiquadFilterRecordSIMD a0;
	BiquadFilterRecordSIMD a1;
	BiquadFilterRecordSIMD a2;
	BiquadFilterRecordSIMD b0;
	BiquadFilterRecordSIMD b1;
	BiquadFilterRecordSIMD b2;

	// for data convolution.
	BiquadFilterRecordSIMD y1;
	BiquadFilterRecordSIMD y2;
	BiquadFilterRecordSIMD x1;
	BiquadFilterRecordSIMD x2;
};

typedef BiquadFilterBuffer Biquad;
typedef BiquadFilterBuffer* PBiquad;

typedef BiquadFilterRecordSIMD BiquadRecord8;
typedef BiquadFilterRecordSIMD* PBiquadRecord8;

typedef BiquadFilterBufferSIMD Biquad8;
typedef BiquadFilterBufferSIMD* PBiquad8;

void InitParamsLPF(PBiquad buffer, float f, float fs);
void InitParamsHPF(PBiquad buffer, float f, float fs);
void InitParamsBPF(PBiquad buffer, float f, float fs);
void InitParamsPF(PBiquad buffer, float f, float fs);
void InitParamsLSF(PBiquad buffer, float f, float fs);
void InitParamsHSF(PBiquad buffer, float f, float fs);

void InitParamsLPF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs);
void InitParamsHPF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs);
void InitParamsBPF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs);
void InitParamsPF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs);
void InitParamsLSF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs);
void InitParamsHSF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs);

// 버퍼 공간의 파라미터가 바뀌지 않습니다.
void SetParamsLPF(PBiquad buffer, float q, float f, float fs);
void SetParamsHPF(PBiquad buffer, float q, float f, float fs);
void SetParamsBPF(PBiquad buffer, float q, float f, float fs);
void SetParamsPF(PBiquad buffer, float q, float f, float fs, float gain);
void SetParamsLSF(PBiquad buffer, float q, float f, float fs, float gain);
void SetParamsHSF(PBiquad buffer, float q, float f, float fs, float gain);

void SetParamsLPF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs);
void SetParamsHPF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs);
void SetParamsBPF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs);
void SetParamsPF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain);
void SetParamsLSF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain);
void SetParamsHSF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain);

// 버퍼 공간의 파라미터가 바뀝니다.
void SetParamsLPF_Safe(PBiquad buffer, float q, float f, float fs);
void SetParamsHPF_Safe(PBiquad buffer, float q, float f, float fs);
void SetParamsBPF_Safe(PBiquad buffer, float q, float f, float fs);
void SetParamsPF_Safe(PBiquad buffer, float q, float f, float fs, float gain);
void SetParamsLSF_Safe(PBiquad buffer, float q, float f, float fs, float gain);
void SetParamsHSF_Safe(PBiquad buffer, float q, float f, float fs, float gain);

void SetParamsLPF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs);
void SetParamsHPF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs);
void SetParamsBPF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs);
void SetParamsPF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain);
void SetParamsLSF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain);
void SetParamsHSF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain);

int16_t BiquadConvolution(PBiquad buffer, int16_t pcmSample);
void BiquadConvolution(PBiquad8 data, PBiquad8 samples, PBiquad8 output);

static int TryUpdateParameter(PBiquad buffer, float value, int index);

static int16_t ClampPCM16(float value);