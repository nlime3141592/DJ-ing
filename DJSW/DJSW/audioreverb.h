#pragma once

#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <assert.h>

// 44100Hz = 272ms
// 48000Hz = 250ms
#define BUFFER_CAPACITY 12000

class SchroederReverb_Comb
{
public:
    SchroederReverb_Comb();
    ~SchroederReverb_Comb() = default;

    void ClearBuffer();
    void SetDelay(float delay_ms, int32_t sampleRate);
    void SetFeedback(float feedback);
    void SetDamping(float damping);
    float Process(float input);

private:
    float xBuffer[BUFFER_CAPACITY];
    float yBuffer[BUFFER_CAPACITY];
    float lpfBuffer;
    int32_t index;
    int32_t sampleRate;
    float delay_ms;
    int32_t delay_sample;
    float feedback;
    float damping, dampingInverse;
};

class SchroederReverb_Allpass
{
public:
    SchroederReverb_Allpass();
    ~SchroederReverb_Allpass() = default;

    void ClearBuffer();
    void SetDelay(float delay_ms, int32_t sampleRate);
    void SetFeedback(float feedback);
    float Process(float input);

private:
    float xBuffer[BUFFER_CAPACITY];
    float yBuffer[BUFFER_CAPACITY];
    size_t index;
    int32_t sampleRate;
    float delay_ms;
    int32_t delay_sample;
    float feedback;
};

class SchroederReverb
{
public:
    SchroederReverb(int sampleRate = 44100);
    ~SchroederReverb() = default;

    void ClearBuffer();
    void SetRoomSize(float roomSize);
    void SetDamping(float damping);
    void SetWet(float wet);
    float Process(float input);
    void ProcessBuffer(const float* inBuf, float* outBuf, size_t n);

private:
    float roomSize;
    float damping;
    float wet;
    float dry;
    std::vector<float> combDelaysMs;
    std::vector<float> allpassDelaysMs;
    std::vector<SchroederReverb_Comb> combs;
    std::vector<SchroederReverb_Allpass> allpasses;
};