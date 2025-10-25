#pragma once

#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <iostream>

// schroeder.cpp
// Simple Schroeder reverb: 4 feedback combs (parallel) -> 2 allpass (series).
// Mono processing. API: processSample(float) or processBuffer(vector<float>&).

class Comb
{
public:
    Comb();
    ~Comb();

    void init(size_t size);
    void setParams(float feedbackGain, float damping);
    float process(float input);

private:
    float* buffer;
    size_t bufferSize;
    size_t index;
    float feedback;
    float filterStore;
    float damp1, damp2;
};

class Allpass
{
public:
    Allpass();
    ~Allpass();

    void init(size_t size);
    void setFeedback(float fb);
    float process(float input);

private:
    float* buffer;
    size_t bufferSize;
    size_t index;
    float feedback;
};

class SchroederReverb
{
public:
    SchroederReverb(int sampleRate = 44100);
    ~SchroederReverb() = default;

    void setParams(float roomSize_, float damping_, float wet_, float dry_);
    float processSample(float in);
    void processBuffer(const float* inBuf, float* outBuf, size_t n);
    void reset();

private:
    int sr;
    float roomSize, damping, wet, dry;
    std::vector<float> combDelaysMs;
    std::vector<float> allpassDelaysMs;
    std::vector<Comb> combs;
    std::vector<Allpass> allpasses;

    void initBuffers();

    static size_t msToSamples(float ms, int sampleRate);
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