#include "audiochannel.h"

#include <assert.h>

static float _hanningWindows[DJSW_WSOLA_FRAME_SIZE];

AudioChannel::AudioChannel() :
	fileData(NULL),
	data(NULL),
	fileSize(0),

	numWavSamples(0),
	wavSamples(NULL),
	position(0),
	olaPosition(0),
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

	masterVolume(0.0f),

	hopDistance(0),
	tshDistance(0)
{
	memset(_wsolaBuffer, 0x00, sizeof(_wsolaBuffer));
}

bool AudioChannel::IsLoaded()
{
	return numWavSamples > 0;
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

	data = (djWavFileHeader*)fileData;

	assert(IsValidWavFile(data));

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
	bool result = HeapFree(GetProcessHeap(), 0, fileData);

	fileData = NULL;
	data = NULL;
	fileSize = 0;

	numWavSamples = 0;
	wavSamples = NULL;
	position = 0;
	isPlaying = false; // TODO: 동기화 문제를 고려한 변수 설정이 필요함.

	return result;
}

void AudioChannel::Play()
{
	olaPosition = 0;

	int16_t buffer[DJSW_WSOLA_FRAME_SIZE];
	memcpy(buffer, wavSamples, sizeof(buffer));
	HanningWindow(buffer);

	int half = DJSW_WSOLA_FRAME_SIZE / 2;

	for (int i = 0; i < half; ++i)
	{
		_wsolaBuffer[i] = wavSamples[position + i];
		_wsolaBuffer[half + i] = buffer[half + i];
	}

	_wsolaPrevFrameIndex = position;

	isPlaying = true;
}

void AudioChannel::Pause()
{
	isPlaying = false;
	position += olaPosition;
	olaPosition = 0;
}

void AudioChannel::Read16(int16_t* out)
{
	// return 16;
}

void AudioChannel::Read2(int16_t* out)
{
	if (!isPlaying)
	{
		if (tshDistance != 0)
		{
			position += tshDistance;
			tshDistance = 0;
		}

		out[0] = 0;
		out[1] = 0;
		return;
	}
	else if (position >= numWavSamples || position < 0)
	{
		out[0] = 0;
		out[1] = 0;
		return;
	}

	//int16_t lSample = (int16_t)wavSamples[position++];
	int16_t lSample = (int16_t)_wsolaBuffer[olaPosition++];
	//int16_t rSample = (int16_t)wavSamples[position++];
	int16_t rSample = (int16_t)_wsolaBuffer[olaPosition++];

	if (olaPosition >= DJSW_WSOLA_OVERLAP_SIZE)
	{
		position += DJSW_WSOLA_OVERLAP_SIZE + hopDistance + tshDistance;
		tshDistance = 0;
		olaPosition = 0;
		WaveformSimilarityOLA();
	}
	
	if (xFadeSampleLeft > 0)
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
	out[0] = lSample;
	out[1] = rSample;

	// return 2;
}

void AudioChannel::ReadPass2(int16_t* out)
{
	position += 2;

	out[0] = 0;
	out[1] = 0;

	// return 2;
}

void AudioChannel::Read(int16_t* out)
{
	if (!isPlaying || position >= numWavSamples)
	{
		out[0] = 0;
		out[1] = 0;
		return;
	}
	else if (isMuted)
	{
		ReadPass2(out);
		return;
	}

	Read2(out);
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

djAudioSource* AudioChannel::GetSource()
{
	return &_audioSource;
}

void AudioChannel::HanningWindow(int16_t* buffer)
{
	for (int i = 0; i < DJSW_WSOLA_FRAME_SIZE; ++i)
	{
		float value = (float)buffer[i] * _hanningWindows[i];
		buffer[i] = (int16_t)value;
	}
}

int32_t AudioChannel::CrossCorrelation(int16_t* buffer0, int16_t* buffer1, int length)
{
	float sum = 0.0f;

	int16_t b0[DJSW_WSOLA_FRAME_SIZE];
	int16_t b1[DJSW_WSOLA_FRAME_SIZE];

	memcpy(b0, buffer0, sizeof(b0));
	memcpy(b1, buffer1, sizeof(b1));

	HanningWindow(b0);
	HanningWindow(b1);

	for (int i = 0; i < length; ++i)
	{
		float v0 = (float)b0[i] / 32768.0f;
		float v1 = (float)b1[i] / 32768.0f;

		sum += v0 * v1;
	}

	return sum;
}

int32_t AudioChannel::SeekBestOverlapPosition(int32_t toleranceRange)
{
	float maxCorrelation = FLT_MIN;
	int32_t maxOffset = 0;

	int16_t* prevBuffer = (int16_t*)wavSamples + _wsolaPrevFrameIndex;
	int16_t* nextBuffer = (int16_t*)wavSamples + position;

	for (int32_t i = -toleranceRange; i < toleranceRange; i += 2)
	{
		float correlation = CrossCorrelation(prevBuffer, nextBuffer + i, DJSW_WSOLA_FRAME_SIZE);

		if (correlation > maxCorrelation)
		{
			maxCorrelation = correlation;
			maxOffset = i;
		}
	}

	return maxOffset;
}

void AudioChannel::WaveformSimilarityOLA()
{
	int frameSize = DJSW_WSOLA_FRAME_SIZE;
	int frameSizeHalf = DJSW_WSOLA_OVERLAP_SIZE;

	if (position + DJSW_WSOLA_FRAME_SIZE > numWavSamples)
		frameSize = numWavSamples - position;

	//int32_t tolerance = DJSW_WSOLA_TOLERANCE_RANGE;
	int32_t tolerance = (int32_t)(0.2f * (float)(hopDistance));
	int32_t offset = SeekBestOverlapPosition(tolerance);

	int16_t* input = (int16_t*)wavSamples + position + offset;

	int16_t buffer[DJSW_WSOLA_FRAME_SIZE];
	memcpy(buffer, input, sizeof(buffer));
	HanningWindow(buffer);

	for (int i = 0; i < frameSizeHalf; ++i)
	{
		_wsolaBuffer[i] = _wsolaBuffer[i + frameSizeHalf] + buffer[i];
		_wsolaBuffer[i + frameSizeHalf] = buffer[i + frameSizeHalf];
	}

	_wsolaPrevFrameIndex = position + offset;
}

void InitAudioChannel()
{
	for (int i = 0; i < DJSW_WSOLA_FRAME_SIZE; ++i)
	{
		_hanningWindows[i] = 0.5f - 0.5f * cosf(2.0f * PI_F * (float)i / (float)(DJSW_WSOLA_FRAME_SIZE - 1));
	}
}