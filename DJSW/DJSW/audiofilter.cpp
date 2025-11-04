#include "audiofilter.h"

static float _initQ[8] = {
	INV_SQRT_2,
	INV_SQRT_2,
	INV_SQRT_2,
	INV_SQRT_2,
	INV_SQRT_2,
	INV_SQRT_2,
	INV_SQRT_2
};

static float _initGain[8] = {
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f
};

void InitParamsLPF(PBiquad buffer, float f, float fs)
{
	buffer->fs = -1.0f; // salt
	SetParamsLPF_Safe(buffer, INV_SQRT_2, f, fs);
}

void InitParamsHPF(PBiquad buffer, float f, float fs)
{
	buffer->fs = -1.0f; // salt
	SetParamsHPF_Safe(buffer, INV_SQRT_2, f, fs);
}

void InitParamsBPF(PBiquad buffer, float f, float fs)
{
	buffer->fs = -1.0f; // salt
	SetParamsBPF_Safe(buffer, INV_SQRT_2, f, fs);
}

void InitParamsPF(PBiquad buffer, float f, float fs)
{
	buffer->fs = -1.0f; // salt
	SetParamsPF_Safe(buffer, 1.0f, f, fs, 0.0f);
}

void InitParamsLSF(PBiquad buffer, float f, float fs)
{
	buffer->fs = -1.0f; // salt
	SetParamsLSF_Safe(buffer, INV_SQRT_2, f, fs, 0.0f);
}

void InitParamsHSF(PBiquad buffer, float f, float fs)
{
	buffer->fs = -1.0f; // salt
	SetParamsHSF_Safe(buffer, INV_SQRT_2, f, fs, 0.0f);
}

void InitParamsLPF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs)
{
	_mm256_storeu_ps((float*)(&buffer->fs), _mm256_set1_ps(-1.0f));
	SetParamsLPF_Safe(buffer, (PBiquadRecord8)_initQ, f, fs);
}

void InitParamsHPF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs)
{
	_mm256_storeu_ps((float*)(&buffer->fs), _mm256_set1_ps(-1.0f));
	SetParamsHPF_Safe(buffer, (PBiquadRecord8)_initQ, f, fs);
}

void InitParamsBPF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs)
{
	_mm256_storeu_ps((float*)(&buffer->fs), _mm256_set1_ps(-1.0f));
	SetParamsBPF_Safe(buffer, (PBiquadRecord8)_initQ, f, fs);
}

void InitParamsPF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs)
{
	_mm256_storeu_ps((float*)(&buffer->fs), _mm256_set1_ps(-1.0f));
	SetParamsPF_Safe(buffer, (PBiquadRecord8)_initQ, f, fs, (PBiquadRecord8)_initGain);
}

void InitParamsLSF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs)
{
	_mm256_storeu_ps((float*)(&buffer->fs), _mm256_set1_ps(-1.0f));
	SetParamsLSF_Safe(buffer, (PBiquadRecord8)_initQ, f, fs, (PBiquadRecord8)_initGain);
}

void InitParamsHSF(PBiquad8 buffer, PBiquadRecord8 f, PBiquadRecord8 fs)
{
	_mm256_storeu_ps((float*)(&buffer->fs), _mm256_set1_ps(-1.0f));
	SetParamsHSF_Safe(buffer, (PBiquadRecord8)_initQ, f, fs, (PBiquadRecord8)_initGain);
}

void SetParamsLPF(PBiquad buffer, float q, float f, float fs)
{
	float w = 2.0f * PI_F * f / fs;
	float s = sinf(w);
	float c = cosf(w);
	float h = 0.5f * s / q;

	buffer->b0 = 0.5f * (1.0f - c);
	buffer->b1 = 1.0f - c;
	buffer->b2 = 0.5f * (1.0f - c);

	buffer->a0 = 1.0f + h;
	buffer->a1 = -2.0f * c;
	buffer->a2 = 1.0f - h;

	buffer->b0 /= buffer->a0;
	buffer->b1 /= buffer->a0;
	buffer->b2 /= buffer->a0;

	buffer->a1 /= buffer->a0;
	buffer->a2 /= buffer->a0;

	buffer->a0 = 1.0f;
}

void SetParamsLPF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs)
{
	__m256 c1 = _mm256_set1_ps(1.0f);
	__m256 c2 = _mm256_set1_ps(2.0f);
	__m256 c5 = _mm256_set1_ps(0.5f);
	__m256 cp = _mm256_set1_ps(PI_F);
	__m256 cn = _mm256_set1_ps(-0.0f);

	__m256 qVector = _mm256_loadu_ps((const float*)q);
	__m256 fVector = _mm256_loadu_ps((const float*)f);
	__m256 fsVector = _mm256_loadu_ps((const float*)fs);

	__m256 wVector = _mm256_div_ps(_mm256_mul_ps(_mm256_mul_ps(c2, cp), fVector), fsVector);
	__m256 sVector = _mm256_sin_ps(wVector);
	__m256 cVector = _mm256_cos_ps(wVector);
	__m256 hVector = _mm256_div_ps(_mm256_mul_ps(c5, sVector), qVector);

	__m256 b1 = _mm256_sub_ps(c1, cVector);
	__m256 b0 = _mm256_mul_ps(c5, b1);
	__m256 b2 = _mm256_mul_ps(c5, b1);

	__m256 a0 = _mm256_add_ps(c1, hVector);
	__m256 a2 = _mm256_sub_ps(c1, hVector);
	__m256 a1 = _mm256_xor_ps(_mm256_mul_ps(c2, cVector), cn);

	_mm256_storeu_ps((float*)(&buffer->b0), _mm256_div_ps(b0, a0));
	_mm256_storeu_ps((float*)(&buffer->b1), _mm256_div_ps(b1, a0));
	_mm256_storeu_ps((float*)(&buffer->b2), _mm256_div_ps(b2, a0));

	_mm256_storeu_ps((float*)(&buffer->a0), c1);
	_mm256_storeu_ps((float*)(&buffer->a1), _mm256_div_ps(a1, a0));
	_mm256_storeu_ps((float*)(&buffer->a2), _mm256_div_ps(a2, a0));
}

void SetParamsHPF(PBiquad buffer, float q, float f, float fs)
{
	float w = 2.0f * PI_F * f / fs;
	float s = sinf(w);
	float c = cosf(w);
	float h = 0.5f * s / q;

	buffer->b0 = 0.5f * (1.0f + c);
	buffer->b1 = -(1.0f + c);
	buffer->b2 = 0.5f * (1.0f + c);

	buffer->a0 = 1.0f + h;
	buffer->a1 = -2.0f * c;
	buffer->a2 = 1.0f - h;

	buffer->b0 /= buffer->a0;
	buffer->b1 /= buffer->a0;
	buffer->b2 /= buffer->a0;

	buffer->a1 /= buffer->a0;
	buffer->a2 /= buffer->a0;

	buffer->a0 = 1.0f;
}

void SetParamsHPF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs)
{
	__m256 c1 = _mm256_set1_ps(1.0f);
	__m256 c2 = _mm256_set1_ps(2.0f);
	__m256 c5 = _mm256_set1_ps(0.5f);
	__m256 cp = _mm256_set1_ps(PI_F);
	__m256 cn = _mm256_set1_ps(-0.0f);

	__m256 qVector = _mm256_loadu_ps((const float*)q);
	__m256 fVector = _mm256_loadu_ps((const float*)f);
	__m256 fsVector = _mm256_loadu_ps((const float*)fs);

	__m256 wVector = _mm256_div_ps(_mm256_mul_ps(_mm256_mul_ps(c2, cp), fVector), fsVector);
	__m256 sVector = _mm256_sin_ps(wVector);
	__m256 cVector = _mm256_cos_ps(wVector);
	__m256 hVector = _mm256_div_ps(_mm256_mul_ps(c5, sVector), qVector);

	__m256 b1 = _mm256_add_ps(c1, cVector);
	__m256 b0 = _mm256_mul_ps(c5, b1);
	__m256 b2 = _mm256_mul_ps(c5, b1);
	b1 = _mm256_xor_ps(b1, cn);

	__m256 a0 = _mm256_add_ps(c1, hVector);
	__m256 a2 = _mm256_sub_ps(c1, hVector);
	__m256 a1 = _mm256_xor_ps(_mm256_mul_ps(c2, cVector), cn);

	_mm256_storeu_ps((float*)(&buffer->b0), _mm256_div_ps(b0, a0));
	_mm256_storeu_ps((float*)(&buffer->b1), _mm256_div_ps(b1, a0));
	_mm256_storeu_ps((float*)(&buffer->b2), _mm256_div_ps(b2, a0));

	_mm256_storeu_ps((float*)(&buffer->a0), c1);
	_mm256_storeu_ps((float*)(&buffer->a1), _mm256_div_ps(a1, a0));
	_mm256_storeu_ps((float*)(&buffer->a2), _mm256_div_ps(a2, a0));
}

void SetParamsBPF(PBiquad buffer, float q, float f, float fs)
{
	float w = 2.0f * PI_F * f / fs;
	float s = sinf(w);
	float c = cosf(w);
	float h = 0.5f * s / q;

	buffer->b0 = 0.5f * s;
	buffer->b1 = 0.0f;
	buffer->b2 = -0.5f * s;

	buffer->a0 = 1.0f + h;
	buffer->a1 = -2.0f * c;
	buffer->a2 = 1.0f - h;

	buffer->b0 /= buffer->a0;
	buffer->b1 /= buffer->a0;
	buffer->b2 /= buffer->a0;

	buffer->a1 /= buffer->a0;
	buffer->a2 /= buffer->a0;

	buffer->a0 = 1.0f;
}

void SetParamsBPF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs)
{
	__m256 c1 = _mm256_set1_ps(1.0f);
	__m256 c2 = _mm256_set1_ps(2.0f);
	__m256 c5 = _mm256_set1_ps(0.5f);
	__m256 cp = _mm256_set1_ps(PI_F);
	__m256 cn = _mm256_set1_ps(-0.0f);

	__m256 qVector = _mm256_loadu_ps((const float*)q);
	__m256 fVector = _mm256_loadu_ps((const float*)f);
	__m256 fsVector = _mm256_loadu_ps((const float*)fs);

	__m256 wVector = _mm256_div_ps(_mm256_mul_ps(_mm256_mul_ps(c2, cp), fVector), fsVector);
	__m256 sVector = _mm256_sin_ps(wVector);
	__m256 cVector = _mm256_cos_ps(wVector);
	__m256 hVector = _mm256_div_ps(_mm256_mul_ps(c5, sVector), qVector);

	__m256 b0 = _mm256_mul_ps(c5, sVector);
	__m256 b1 = _mm256_set1_ps(0.0f);
	__m256 b2 = _mm256_xor_ps(b0, cn);

	__m256 a0 = _mm256_add_ps(c1, hVector);
	__m256 a2 = _mm256_sub_ps(c1, hVector);
	__m256 a1 = _mm256_xor_ps(_mm256_mul_ps(c2, cVector), cn);

	_mm256_storeu_ps((float*)(&buffer->b0), _mm256_div_ps(b0, a0));
	_mm256_storeu_ps((float*)(&buffer->b1), _mm256_div_ps(b1, a0));
	_mm256_storeu_ps((float*)(&buffer->b2), _mm256_div_ps(b2, a0));

	_mm256_storeu_ps((float*)(&buffer->a0), c1);
	_mm256_storeu_ps((float*)(&buffer->a1), _mm256_div_ps(a1, a0));
	_mm256_storeu_ps((float*)(&buffer->a2), _mm256_div_ps(a2, a0));
}

void SetParamsPF(PBiquad buffer, float q, float f, float fs, float gain)
{
	float w = 2.0f * PI_F * f / fs;
	float s = sinf(w);
	float c = cosf(w);
	float h = 0.5f * s / q;
	float g = powf(10.0f, gain / 40.0f);

	buffer->b0 = 1.0f + h * g;
	buffer->b1 = -2.0f * c;
	buffer->b2 = 1.0f - h * g;

	buffer->a0 = 1.0f + h / g;
	buffer->a1 = -2.0f * c;
	buffer->a2 = 1.0f - h / g;

	buffer->b0 /= buffer->a0;
	buffer->b1 /= buffer->a0;
	buffer->b2 /= buffer->a0;

	buffer->a1 /= buffer->a0;
	buffer->a2 /= buffer->a0;

	buffer->a0 = 1.0f;
}

void SetParamsPF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain)
{
	__m256 c1 = _mm256_set1_ps(1.0f);
	__m256 c2 = _mm256_set1_ps(2.0f);
	__m256 c5 = _mm256_set1_ps(0.5f);
	__m256 cp = _mm256_set1_ps(PI_F);
	__m256 cn = _mm256_set1_ps(-0.0f);

	__m256 qVector = _mm256_loadu_ps((const float*)q);
	__m256 fVector = _mm256_loadu_ps((const float*)f);
	__m256 fsVector = _mm256_loadu_ps((const float*)fs);
	__m256 gainVector = _mm256_loadu_ps((const float*)gain);

	__m256 wVector = _mm256_div_ps(_mm256_mul_ps(_mm256_mul_ps(c2, cp), fVector), fsVector);
	__m256 sVector = _mm256_sin_ps(wVector);
	__m256 cVector = _mm256_cos_ps(wVector);
	__m256 hVector = _mm256_div_ps(_mm256_mul_ps(c5, sVector), qVector);
	__m256 gVector = _mm256_pow_ps(_mm256_set1_ps(10.0f), _mm256_div_ps(gainVector, _mm256_set1_ps(40.0f)));

	__m256 b0 = _mm256_add_ps(c1, _mm256_mul_ps(hVector, gVector));
	__m256 b1 = _mm256_xor_ps(_mm256_mul_ps(c2, cVector), cn);
	__m256 b2 = _mm256_sub_ps(c1, _mm256_mul_ps(hVector, gVector));

	__m256 a0 = _mm256_add_ps(c1, _mm256_div_ps(hVector, gVector));
	__m256 a1 = _mm256_xor_ps(_mm256_mul_ps(c2, cVector), cn);
	__m256 a2 = _mm256_sub_ps(c1, _mm256_div_ps(hVector, gVector));

	_mm256_storeu_ps((float*)(&buffer->b0), _mm256_div_ps(b0, a0));
	_mm256_storeu_ps((float*)(&buffer->b1), _mm256_div_ps(b1, a0));
	_mm256_storeu_ps((float*)(&buffer->b2), _mm256_div_ps(b2, a0));

	_mm256_storeu_ps((float*)(&buffer->a0), c1);
	_mm256_storeu_ps((float*)(&buffer->a1), _mm256_div_ps(a1, a0));
	_mm256_storeu_ps((float*)(&buffer->a2), _mm256_div_ps(a2, a0));
}

void SetParamsLSF(PBiquad buffer, float q, float f, float fs, float gain)
{
	float w = 2.0f * PI_F * f / fs;
	float s = sinf(w);
	float c = cosf(w);
	float h = 0.5f * s / q;
	float g = powf(10.0f, gain / 40.0f);

	float gp = g + 1.0f;
	float gm = g - 1.0f;
	float gh = 2.0f * sqrt(g) * h;

	buffer->b0 = g * (gp - gm * c + gh);
	buffer->b1 = 2.0f * g * (gm - gp * c);
	buffer->b2 = g * (gp - gm * c - gh);

	buffer->a0 = gp + gm * c + gh;
	buffer->a1 = -2.0f * (gm + gp * c);
	buffer->a2 = gp + gm * c - gh;

	buffer->b0 /= buffer->a0;
	buffer->b1 /= buffer->a0;
	buffer->b2 /= buffer->a0;

	buffer->a1 /= buffer->a0;
	buffer->a2 /= buffer->a0;

	buffer->a0 = 1.0f;
}

void SetParamsLSF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain)
{
	__m256 c1 = _mm256_set1_ps(1.0f);
	__m256 c2 = _mm256_set1_ps(2.0f);
	__m256 c5 = _mm256_set1_ps(0.5f);
	__m256 cp = _mm256_set1_ps(PI_F);
	__m256 cn = _mm256_set1_ps(-0.0f);

	__m256 qVector = _mm256_loadu_ps((const float*)q);
	__m256 fVector = _mm256_loadu_ps((const float*)f);
	__m256 fsVector = _mm256_loadu_ps((const float*)fs);
	__m256 gainVector = _mm256_loadu_ps((const float*)gain);

	__m256 wVector = _mm256_div_ps(_mm256_mul_ps(_mm256_mul_ps(c2, cp), fVector), fsVector);
	__m256 sVector = _mm256_sin_ps(wVector);
	__m256 cVector = _mm256_cos_ps(wVector);
	__m256 hVector = _mm256_div_ps(_mm256_mul_ps(c5, sVector), qVector);
	__m256 gVector = _mm256_pow_ps(_mm256_set1_ps(10.0f), _mm256_div_ps(gainVector, _mm256_set1_ps(40.0f)));

	__m256 gpVector = _mm256_add_ps(gVector, c1);
	__m256 gmVector = _mm256_sub_ps(gVector, c1);
	__m256 ghVector = _mm256_mul_ps(_mm256_sqrt_ps(gVector), _mm256_mul_ps(c2, hVector));
	__m256 gpcVector = _mm256_mul_ps(gpVector, cVector);
	__m256 gmcVector = _mm256_mul_ps(gmVector, cVector);

	__m256 b0 = _mm256_mul_ps(gVector, _mm256_sub_ps(_mm256_add_ps(gpVector, ghVector), gmcVector));
	__m256 b1 = _mm256_mul_ps(c2, _mm256_mul_ps(gVector, _mm256_sub_ps(gmVector, gpcVector)));
	__m256 b2 = _mm256_mul_ps(gVector, _mm256_sub_ps(gpVector, _mm256_add_ps(gmcVector, ghVector)));

	__m256 a0 = _mm256_add_ps(gpVector, _mm256_add_ps(gmcVector, ghVector));
	__m256 a1 = _mm256_xor_ps(_mm256_mul_ps(c2, _mm256_add_ps(gmVector, gpcVector)), cn);
	__m256 a2 = _mm256_add_ps(gpVector, _mm256_sub_ps(gmcVector, ghVector));

	_mm256_storeu_ps((float*)(&buffer->b0), _mm256_div_ps(b0, a0));
	_mm256_storeu_ps((float*)(&buffer->b1), _mm256_div_ps(b1, a0));
	_mm256_storeu_ps((float*)(&buffer->b2), _mm256_div_ps(b2, a0));

	_mm256_storeu_ps((float*)(&buffer->a0), c1);
	_mm256_storeu_ps((float*)(&buffer->a1), _mm256_div_ps(a1, a0));
	_mm256_storeu_ps((float*)(&buffer->a2), _mm256_div_ps(a2, a0));
}

void SetParamsHSF(PBiquad buffer, float q, float f, float fs, float gain)
{
	float w = 2.0f * PI_F * f / fs;
	float s = sinf(w);
	float c = cosf(w);
	float h = 0.5f * s / q;
	float g = powf(10.0f, gain / 40.0f);

	float gp = g + 1.0f;
	float gm = g - 1.0f;
	float gh = 2.0f * sqrt(g) * h;

	buffer->b0 = g * (gp + gm * c + gh);
	buffer->b1 = -2.0f * g * (gm + gp * c);
	buffer->b2 = g * (gp + gm * c - gh);

	buffer->a0 = gp - gm * c + gh;
	buffer->a1 = 2.0f * (gm - gp * c);
	buffer->a2 = gp - gm * c - gh;

	buffer->b0 /= buffer->a0;
	buffer->b1 /= buffer->a0;
	buffer->b2 /= buffer->a0;

	buffer->a1 /= buffer->a0;
	buffer->a2 /= buffer->a0;

	buffer->a0 = 1.0f;
}

void SetParamsHSF(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain)
{
	__m256 c1 = _mm256_set1_ps(1.0f);
	__m256 c2 = _mm256_set1_ps(2.0f);
	__m256 c5 = _mm256_set1_ps(0.5f);
	__m256 cp = _mm256_set1_ps(PI_F);
	__m256 cn = _mm256_set1_ps(-0.0f);

	__m256 qVector = _mm256_loadu_ps((const float*)q);
	__m256 fVector = _mm256_loadu_ps((const float*)f);
	__m256 fsVector = _mm256_loadu_ps((const float*)fs);
	__m256 gainVector = _mm256_loadu_ps((const float*)gain);

	__m256 wVector = _mm256_div_ps(_mm256_mul_ps(_mm256_mul_ps(c2, cp), fVector), fsVector);
	__m256 sVector = _mm256_sin_ps(wVector);
	__m256 cVector = _mm256_cos_ps(wVector);
	__m256 hVector = _mm256_div_ps(_mm256_mul_ps(c5, sVector), qVector);
	__m256 gVector = _mm256_pow_ps(_mm256_set1_ps(10.0f), _mm256_div_ps(gainVector, _mm256_set1_ps(40.0f)));

	__m256 gpVector = _mm256_add_ps(gVector, c1);
	__m256 gmVector = _mm256_sub_ps(gVector, c1);
	__m256 ghVector = _mm256_mul_ps(_mm256_sqrt_ps(gVector), _mm256_mul_ps(c2, hVector));
	__m256 gpcVector = _mm256_mul_ps(gpVector, cVector);
	__m256 gmcVector = _mm256_mul_ps(gmVector, cVector);

	__m256 b0 = _mm256_mul_ps(gVector, _mm256_add_ps(_mm256_add_ps(gpVector, ghVector), gmcVector));
	__m256 b1 = _mm256_xor_ps(_mm256_mul_ps(c2, _mm256_mul_ps(gVector, _mm256_add_ps(gmVector, gpcVector))), cn);
	__m256 b2 = _mm256_mul_ps(gVector, _mm256_add_ps(_mm256_sub_ps(gpVector, ghVector), gmcVector));

	__m256 a0 = _mm256_add_ps(gpVector, _mm256_sub_ps(ghVector, gmcVector));
	__m256 a1 = _mm256_mul_ps(c2, _mm256_sub_ps(gmVector, gpcVector));
	__m256 a2 = _mm256_sub_ps(gpVector, _mm256_add_ps(gmcVector, ghVector));

	_mm256_storeu_ps((float*)(&buffer->b0), _mm256_div_ps(b0, a0));
	_mm256_storeu_ps((float*)(&buffer->b1), _mm256_div_ps(b1, a0));
	_mm256_storeu_ps((float*)(&buffer->b2), _mm256_div_ps(b2, a0));

	_mm256_storeu_ps((float*)(&buffer->a0), c1);
	_mm256_storeu_ps((float*)(&buffer->a1), _mm256_div_ps(a1, a0));
	_mm256_storeu_ps((float*)(&buffer->a2), _mm256_div_ps(a2, a0));
}


void SetParamsLPF_Safe(PBiquad buffer, float q, float f, float fs)
{
	int updated = 0;

	updated |= TryUpdateParameter(buffer, q, INDEX_Q);
	updated |= TryUpdateParameter(buffer, f, INDEX_F);
	updated |= TryUpdateParameter(buffer, fs, INDEX_FS);

	if (!updated)
		return;

	SetParamsLPF(buffer, q, f, fs);
}

void SetParamsLPF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs) {}
void SetParamsHPF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs) {}
void SetParamsBPF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs) {}
void SetParamsPF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain) {}
void SetParamsLSF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain) {}
void SetParamsHSF_Safe(PBiquad8 buffer, PBiquadRecord8 q, PBiquadRecord8 f, PBiquadRecord8 fs, PBiquadRecord8 gain) {}

void SetParamsHPF_Safe(PBiquad buffer, float q, float f, float fs)
{
	int updated = 0;

	updated |= TryUpdateParameter(buffer, q, INDEX_Q);
	updated |= TryUpdateParameter(buffer, f, INDEX_F);
	updated |= TryUpdateParameter(buffer, fs, INDEX_FS);

	if (!updated)
		return;

	SetParamsHPF(buffer, q, f, fs);
}

void SetParamsBPF_Safe(PBiquad buffer, float q, float f, float fs)
{
	int updated = 0;

	updated |= TryUpdateParameter(buffer, q, INDEX_Q);
	updated |= TryUpdateParameter(buffer, f, INDEX_F);
	updated |= TryUpdateParameter(buffer, fs, INDEX_FS);

	if (!updated)
		return;

	SetParamsBPF(buffer, q, f, fs);
}

void SetParamsPF_Safe(PBiquad buffer, float q, float f, float fs, float gain)
{
	int updated = 0;

	updated |= TryUpdateParameter(buffer, q, INDEX_Q);
	updated |= TryUpdateParameter(buffer, f, INDEX_F);
	updated |= TryUpdateParameter(buffer, fs, INDEX_FS);
	updated |= TryUpdateParameter(buffer, gain, INDEX_GAIN);

	if (!updated)
		return;

	SetParamsPF(buffer, q, f, fs, gain);
}

void SetParamsLSF_Safe(PBiquad buffer, float q, float f, float fs, float gain)
{
	int updated = 0;

	updated |= TryUpdateParameter(buffer, q, INDEX_Q);
	updated |= TryUpdateParameter(buffer, f, INDEX_F);
	updated |= TryUpdateParameter(buffer, fs, INDEX_FS);
	updated |= TryUpdateParameter(buffer, gain, INDEX_GAIN);

	if (!updated)
		return;

	SetParamsLSF(buffer, q, f, fs, gain);
}

void SetParamsHSF_Safe(PBiquad buffer, float q, float f, float fs, float gain)
{
	int updated = 0;

	updated |= TryUpdateParameter(buffer, q, INDEX_Q);
	updated |= TryUpdateParameter(buffer, f, INDEX_F);
	updated |= TryUpdateParameter(buffer, fs, INDEX_FS);
	updated |= TryUpdateParameter(buffer, gain, INDEX_GAIN);

	if (!updated)
		return;

	SetParamsHSF(buffer, q, f, fs, gain);
}

int16_t BiquadConvolution(PBiquad buffer, int16_t pcmSample)
{
	float v = pcmSample;

	float output = v * buffer->b0 + buffer->x1 * buffer->b1 + buffer->x2 * buffer->b2
		- buffer->y1 * buffer->a1 - buffer->y2 * buffer->a2;

	buffer->x2 = buffer->x1;
	buffer->x1 = v;
	buffer->y2 = buffer->y1;
	buffer->y1 = output;

	return ClampPCM16(output);
}

void BiquadConvolution(PBiquad8 data, PBiquad8 samples, PBiquad8 output)
{
	__m256 vVector = _mm256_loadu_ps((const float*)samples);

	__m256 b0 = _mm256_loadu_ps((const float*)(&data->b0));
	__m256 b1 = _mm256_loadu_ps((const float*)(&data->b1));
	__m256 b2 = _mm256_loadu_ps((const float*)(&data->b2));
	__m256 a1 = _mm256_loadu_ps((const float*)(&data->a1));
	__m256 a2 = _mm256_loadu_ps((const float*)(&data->a2));

	__m256 x1 = _mm256_loadu_ps((const float*)(&data->x1));
	__m256 x2 = _mm256_loadu_ps((const float*)(&data->x2));
	__m256 y1 = _mm256_loadu_ps((const float*)(&data->y1));
	__m256 y2 = _mm256_loadu_ps((const float*)(&data->y2));

	__m256 oVector = _mm256_mul_ps(vVector, b0);
	oVector = _mm256_add_ps(oVector, _mm256_mul_ps(x1, b1));
	oVector = _mm256_add_ps(oVector, _mm256_mul_ps(x2, b2));
	oVector = _mm256_sub_ps(oVector, _mm256_mul_ps(y1, a1));
	oVector = _mm256_sub_ps(oVector, _mm256_mul_ps(y2, a2));

	_mm256_storeu_ps((float*)(&data->x2), x1);
	_mm256_storeu_ps((float*)(&data->x1), vVector);
	_mm256_storeu_ps((float*)(&data->y2), y1);
	_mm256_storeu_ps((float*)(&data->y1), oVector);

	_mm256_storeu_ps((float*)output, oVector);
}

static int TryUpdateParameter(PBiquad buffer, float value, int index)
{
	float* parameters = &(buffer->q);

	if (parameters[index] == value)
		return 0;

	parameters[index] = value;
	return 1;
}

static int16_t ClampPCM16(float value)
{
	if (value < -32768.0f)
		return (int16_t)-32768.0f;
	else if (value > 32767.0f)
		return (int16_t)32767.0f;
	else
		return (int16_t)value;
}