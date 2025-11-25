#pragma once

#include <string>

#include "djsw_file_metadata.h"
#include "djsw_file_wav.h"

using namespace std;

#define DJSW_BAR_COUNT_0 0
#define DJSW_BAR_COUNT_1 1
#define DJSW_BAR_COUNT_2 2
#define DJSW_BAR_COUNT_4 4
#define DJSW_BAR_COUNT_8 8
#define DJSW_BAR_COUNT_16 16
#define DJSW_BAR_COUNT_32 32
#define DJSW_BAR_COUNT_64 64

#define DJSW_BAR_COUNT_2_INVERSE -2
#define DJSW_BAR_COUNT_4_INVERSE -4
#define DJSW_BAR_COUNT_8_INVERSE -8
#define DJSW_BAR_COUNT_16_INVERSE -16
#define DJSW_BAR_COUNT_32_INVERSE -32
#define DJSW_BAR_COUNT_64_INVERSE -64

#define _DJSW_WSOLA_FRAME_SIZE 256 // even number, prefer n-power of 2.
#define _DJSW_WSOLA_OVERLAP_SIZE 128 // even number, MUST BE half of frame size
#define _DJSW_WSOLA_MAX_TOLERANCE 20

#define DJSW_CROSSFADE_SAMPLE_LENGTH 100

#define DJSW_SAMPLE_DATA_TYPE int16_t
#define DJSW_BYTES_PER_SAMPLE sizeof(DJSW_SAMPLE_DATA_TYPE)

typedef DJSW_SAMPLE_DATA_TYPE* djSamplePtr_t;

class djAudioSource
{
public:
	djAudioSource();
	~djAudioSource();

	wstring CreateMetadata(wstring wavPath);
	bool IsLoaded();
	bool Load(wstring metadataPath);
	bool Unload();

	int32_t GetGlobalCueIndex();
	void SetGlobalCueIndex(int32_t index);
	
	bool IsLoop();
	void SetLoop(int32_t loopBarCount, bool shouldQuantize);
	
	void Jump(int32_t jumpIndex);

	void ClearHotCue(int hotCueIndex);
	void SetHotCue(int hotCueIndex);
	int32_t GetHotCue(int hotCueIndex);
	
	void ReadInit();
	void Read(int16_t* out);
	void Peek(int32_t scale, int32_t offset, int32_t idxChannel, int16_t* outMin, int16_t* outMax);

	bool IsPlaying();

	void Play();
	void Pause();

	void SetTimeShiftDistance(int32_t timeShiftSamples);
	void SetHopDistance(int32_t hopDistance);

	int16_t GetNumChannels();
	char* GetWavFilePath();

private:
	djWavMetaFile _metaFile;
	djWavFileHeader* _header;
	int32_t _audioFileSize;

	int32_t _numWavSamples;
	int16_t* _wavSamples;

	int32_t _glbPosition;
	int32_t _olaPosition;

	bool _isPlaying;

	bool _useGlobalCue;
	int32_t _glbCueIndex;

	bool _shouldJump;
	int32_t _jumpIndex;

	bool _useLoop;
	int32_t _loopBarCount;
	int32_t _loopIndex;
	int32_t _loopLength;

	int32_t _hopDistance;
	int32_t _tshDistance;

	int32_t _wsolaInputSize;
	int16_t* _wsolaInputBuffer0;
	int16_t* _wsolaInputBuffer1;
	int32_t LoadInputBuffer(int16_t* inputBuffer, int32_t originPosition);

	int32_t _wsolaHannBufferSize;
	int16_t* _wsolaHannedValueBuffer;
	float* _wsolaHanningWindowBuffer;
	void ApplyHanningWindow(int16_t* buffer, int32_t begin, int32_t length);

	int32_t _wsolaOutputSize;
	int16_t* _wsolaOutputBuffer;

	int32_t _wsolaSelectedTolerance;
	float GetCrossCorrelation(int16_t* a, int16_t* b, int32_t length);
	int32_t SeekBestOverlapPosition(int32_t tolerance);

	int32_t _xFadeBeg;
	int32_t _xFadeSampleLeft;
};