#pragma once

#include <stdint.h>
#include <stdio.h>

typedef struct _autocorrelation
{
	float bpm;
	int offset;
	int tau;
} AutoCorrelation;

float GetAutoCorrelation(
	AutoCorrelation* output,
	float bpmMin,
	float bpmMax,
	float bpmDelta,
	int16_t* buffer,
	int offset,
	int length,
	int fs,
	int channel);

static constexpr float AccentFilterCoef1(float w0, float w1);
static constexpr float AccentFilterCoef2(float w0, float w1);
static constexpr float AccentFilterCoef3(float w0, float w1);
static constexpr float AccentFilterCoef4(float w0, float w1);

static float AccentFilter(float tau, float* buffer, int offset, int length, int channel);