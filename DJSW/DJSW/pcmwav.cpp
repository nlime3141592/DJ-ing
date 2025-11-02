#include "pcmwav.h"

void ValidateFile_Debug(WavFile_PCM* file)
{
    // Check the Chunk IDs to make sure we loaded the file correctly
    assert(file->riffId == 1179011410);
    assert(file->waveId == 1163280727);
    assert(file->fmtId == 544501094);
    assert(file->dataId == 1635017060);

    // Check data is in format we expect
    assert(file->formatCode == 1); // Only support PCM data
    assert(file->numChannels == 2); // Only support 2-channel data
    assert(file->fmtChunkSize == 16); // This should be true for PCM data
    assert(file->sampleRate == 44100); // Only support 44100Hz data
    assert(file->bitsPerSample == 16); // Only support 16-bit samples

    // This is how these fields are defined, no harm to assert that they're what we expect
    assert(file->blockAlign == file->numChannels * file->bitsPerSample / 8);
    assert(file->byteRate == file->sampleRate * file->blockAlign);
}