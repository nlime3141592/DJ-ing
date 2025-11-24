#pragma once

#include <stdint.h>

struct djWavFileHeader
{
    // RIFF Chunk
    uint32_t riffId;
    uint32_t riffChunkSize;
    uint32_t waveId;

    // fmt Chunk
    uint32_t fmtId;
    uint32_t fmtChunkSize;
    uint16_t formatCode;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;

    // data Chunk
    uint32_t dataId;
    uint32_t dataChunkSize;
    int16_t samples[];
};

bool LoadWavFile(char* wavFilePath, djWavFileHeader** header, int32_t* fileSize);
bool IsValidWavFile(djWavFileHeader* header);