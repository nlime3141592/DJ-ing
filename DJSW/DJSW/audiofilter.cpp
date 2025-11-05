#include "audiofilter.h"

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