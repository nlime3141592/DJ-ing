#include "audiochannel.h"

AudioChannel::AudioChannel() :
	position(0),
	isPlaying(false),
	isMuted(false),

	xFadeBeg(0),
	xFadeSampleLeft(0),

	fxNumber(0),
	fxReverbL(SchroederReverb()),
	fxReverbR(SchroederReverb()),

	eqLowL(Biquad()),
	eqLowR(Biquad()),
	eqMidL(Biquad()),
	eqMidR(Biquad()),
	eqHighL(Biquad()),
	eqHighR(Biquad()),

	loopBeg(0),
	loopLength(0),

	masterVolume(0.0f)
{

}

bool AudioChannel::Load(const char* fileName)
{
	HANDLE file = CreateFileA(fileName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

	if (file == INVALID_HANDLE_VALUE)
		return false;

	DWORD fileSize = GetFileSize(file, 0);
	
	if (!fileSize)
		return false;

	fileData = HeapAlloc(GetProcessHeap(), 0, fileSize + 1);

	if (!fileData)
		return false;

	if (!ReadFile(file, fileData, fileSize, (LPDWORD)&fileSize, 0))
		return false;

	CloseHandle(file);
	((uint8_t*)fileData)[fileSize] = 0;

	data = (WavFile_PCM*)fileData;

	ValidateFile_Debug(data);

	numWavSamples = data->dataChunkSize / sizeof(uint16_t);
	wavSamples = data->samples;

	return true;
}

void AudioChannel::Reset()
{
	xFadeBeg = 0;
	xFadeSampleLeft = 0;

	fxNumber = 0;

	loopBeg = 0;
	loopLength = 0;
}

bool AudioChannel::Unload()
{
	return HeapFree(GetProcessHeap(), 0, fileData);
}

void AudioChannel::Play()
{
	isPlaying = true;
}

void AudioChannel::Pause()
{
	isPlaying = false;
}

bool AudioChannel::Read(int16_t* lSampleOutput, int16_t* rSampleOutput)
{
	if (!isPlaying || position >= numWavSamples)
	{
		*lSampleOutput = 0;
		*rSampleOutput = 0;

		return true;
	}

	int16_t lSample = (int16_t)wavSamples[position++];
	int16_t rSample = (int16_t)wavSamples[position++];

	// Crossfade Inputs
	if (isMuted)
	{
		lSample = 0.0f;
		rSample = 0.0f;
		return true;
	}
	else if (xFadeSampleLeft > 0)
	{
		float w0 = (float)xFadeSampleLeft / xFadeSampleLength;
		float w1 = 1.0f - w0;

		int32_t d0 = xFadeSampleLength - xFadeSampleLeft;
		int32_t d1 = xFadeSampleLeft;

		int32_t i0 = xFadeBeg + d0;
		int32_t i1 = position - d1;

		int16_t s0 = 0;
		int16_t s1 = 0;

		if (i0 < numWavSamples)
			s0 = wavSamples[i0];
		if (i1 >= 0)
			s1 = wavSamples[i1];

		lSample = w0 * s0 + w1 * s1;

		xFadeSampleLeft -= 2; // because of stereo channel
	}

	// Pad FX
	switch (fxNumber)
	{
	case 0:
		// No FX
		break;
	case 1:
		// Schroeder Reverb
		lSample = fxReverbL.Process(lSample);
		rSample = fxReverbR.Process(rSample);
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	default:
		break;
	}

	// EQ
	//lSample = BiquadConvolution(&eqLowL, lSample);
	//lSample = BiquadConvolution(&eqMidL, lSample);
	//lSample = BiquadConvolution(&eqHighL, lSample);

	//rSample = BiquadConvolution(&eqLowR, rSample);
	//rSample = BiquadConvolution(&eqMidR, rSample);
	//rSample = BiquadConvolution(&eqHighR, rSample);

	// Loop
	if (loopLength > 0 && position >= loopBeg + loopLength)
	{
		int32_t backup_beg = loopBeg;
		int32_t backup_len = loopLength;

		JumpImmediate(loopBeg);

		loopBeg = backup_beg;
		loopLength = backup_len;
	}

	// Master Volume
	lSample = (int16_t)((float)lSample * masterVolume);
	rSample = (int16_t)((float)rSample * masterVolume);

	// Final Outputs
	*lSampleOutput = lSample;
	*rSampleOutput = rSample;
}

void AudioChannel::Jump(int32_t sampleJumpingTo, int32_t whatSampleJumpingFrom)
{
	assert(position <= whatSampleJumpingFrom);
}

void AudioChannel::JumpImmediate(int32_t sampleJumpingTo)
{
	xFadeBeg = position;
	xFadeSampleLeft = xFadeSampleLength;

	position = sampleJumpingTo;
}

void AudioChannel::SetLoop(int32_t begin, int32_t length)
{
	loopBeg = begin;
	loopLength = length;
}

void AudioChannel::ClearLoop()
{
	loopBeg = 0;
	loopLength = 0;
}