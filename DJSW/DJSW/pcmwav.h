#pragma once

#include <assert.h>
#include <stdint.h>

#pragma warning(disable: 4200)
// Struct to get data from loaded WAV file.
// NB: This will only work for WAV files containing PCM (non-compressed) data
// otherwise the layout will be different.
struct WavFile_PCM
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

    // These are not present for PCM Wav Files
    // uint16_t cbSize;
    // uint16_t wValidBitsPerSample;
    // uint32_t dwChannelMask;
    // char subFormatGUID[16];

    // data Chunk
    uint32_t dataId;
    uint32_t dataChunkSize;
    uint16_t samples[]; // actual samples start here
};
#pragma warning(default: 4200)

void ValidateFile_Debug(WavFile_PCM* file);