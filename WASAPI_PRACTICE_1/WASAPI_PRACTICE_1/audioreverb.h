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

class Comb
{
public:
    Comb();
    ~Comb() = default;

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

class Allpass
{
public:
    Allpass();
    ~Allpass() = default;
    
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
    int sr;
    float roomSize, damping, wet, dry;
    std::vector<float> combDelaysMs;
    std::vector<float> allpassDelaysMs;
    std::vector<Comb> combs;
    std::vector<Allpass> allpasses;

    float GetRoomSizeInternal(float roomSizeExternal);
};

// -------------------- Usage example (main) --------------------
#ifdef SCHROEDER_MAIN_EXAMPLE
#include <fstream>
#include <vector>

// A tiny example that processes a sine impulse buffer and prints some samples.
// In real use, feed from an audio file or audio callback.

int main() {
    const int sr = 44100;
    SchroederReverb reverb(sr);
    reverb.setParams(0.8f, 0.2f, 0.35f, 0.65f); // bigger room

    // generate a short impulse (1 second)
    int N = sr;
    std::vector<float> in(N, 0.0f), out(N, 0.0f);
    in[0] = 1.0f; // impulse

    reverb.processBuffer(in.data(), out.data(), N);

    // print first 50 samples
    for (int i = 0; i < 50; ++i) {
        std::cout << out[i] << "\n";
    }
    // optionally write raw float file or integrate with audio library
    return 0;
}
#endif