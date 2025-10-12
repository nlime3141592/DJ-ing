#pragma once
#pragma once

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

typedef BiquadFilterBuffer Biquad;
typedef BiquadFilterBuffer* PBiquad;

void InitParamsLPF(PBiquad buffer, float f, float fs);
void InitParamsHPF(PBiquad buffer, float f, float fs);
void InitParamsBPF(PBiquad buffer, float f, float fs);
void InitParamsPF(PBiquad buffer, float f, float fs);
void InitParamsLSF(PBiquad buffer, float f, float fs);
void InitParamsHSF(PBiquad buffer, float f, float fs);

// 버퍼 공간의 파라미터가 바뀌지 않습니다.
void SetParamsLPF(PBiquad buffer, float q, float f, float fs);
void SetParamsHPF(PBiquad buffer, float q, float f, float fs);
void SetParamsBPF(PBiquad buffer, float q, float f, float fs);
void SetParamsPF(PBiquad buffer, float q, float f, float fs, float gain);
void SetParamsLSF(PBiquad buffer, float q, float f, float fs, float gain);
void SetParamsHSF(PBiquad buffer, float q, float f, float fs, float gain);

// 버퍼 공간의 파라미터가 바뀝니다.
void SetParamsLPF_Safe(PBiquad buffer, float q, float f, float fs);
void SetParamsHPF_Safe(PBiquad buffer, float q, float f, float fs);
void SetParamsBPF_Safe(PBiquad buffer, float q, float f, float fs);
void SetParamsPF_Safe(PBiquad buffer, float q, float f, float fs, float gain);
void SetParamsLSF_Safe(PBiquad buffer, float q, float f, float fs, float gain);
void SetParamsHSF_Safe(PBiquad buffer, float q, float f, float fs, float gain);

int16_t BiquadConvolution(PBiquad buffer, int16_t pcmSample);

static int TryUpdateParameter(PBiquad buffer, float value, int index);

static int16_t ClampPCM16(float value);