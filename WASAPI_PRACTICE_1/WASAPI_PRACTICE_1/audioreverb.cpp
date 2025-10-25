#include "audioreverb.h"

Comb::Comb() : 
    buffer(nullptr),
    bufferSize(0),
    index(0),
    feedback(0.0f),
    filterStore(0.0f),
    damp1(0.0f),
    damp2(0.0f)
{

}

Comb::~Comb()
{
    delete[] buffer;
}

void Comb::init(size_t size)
{
    delete[] buffer;
    bufferSize = size;
    buffer = new float[bufferSize];
    std::fill(buffer, buffer + bufferSize, 0.0f);
    index = 0;
    filterStore = 0.0f;
}

// set feedback gain (0..1) and damping (0..1)
void Comb::setParams(float feedbackGain, float damping)
{
    feedback = feedbackGain;
    // damping uses a simple one-pole lowpass in the feedback loop
    damp1 = damping;
    damp2 = 1.0f - damping;
}

// process single sample
float Comb::process(float input)
{
    // read delayed sample
    float output = buffer[index];

    // lowpass in feedback path (simple one-pole)
    filterStore = (output * damp2) + (filterStore * damp1);

    // new sample into buffer = input + feedback * filtered
    buffer[index] = input + filterStore * feedback;

    // advance circular index
    if (++index >= bufferSize) index = 0;

    return output;
}

Allpass::Allpass() : 
    buffer(nullptr),
    bufferSize(0),
    index(0),
    feedback(0.5f)
{

}

Allpass::~Allpass()
{
    delete[] buffer;
}

void Allpass::init(size_t size)
{
    delete[] buffer;
    bufferSize = size;
    buffer = new float[bufferSize];
    std::fill(buffer, buffer + bufferSize, 0.0f);
    index = 0;
}

void Allpass::setFeedback(float fb)
{
    feedback = fb;
}

float Allpass::process(float input)
{
    float bufout = buffer[index];
    float output = -input + bufout;
    buffer[index] = input + bufout * feedback;
    if (++index >= bufferSize) index = 0;
    return output;
}

SchroederReverb::SchroederReverb(int sampleRate) :
    sr(sampleRate),
    roomSize(0.5f),
    damping(0.2f),
    wet(0.33f),
    dry(0.67f)
{
    // delay times in milliseconds (tunable)
        // these are chosen to be relatively prime-ish to avoid metallic resonances
    combDelaysMs = { 50.0f, 56.0f, 61.0f, 68.0f }; // comb delays in ms
    allpassDelaysMs = { 6.0f, 12.0f };            // allpass delays in ms

    combs.resize(combDelaysMs.size());
    allpasses.resize(allpassDelaysMs.size());

    initBuffers();
    setParams(roomSize, damping, wet, dry);
}

void SchroederReverb::setParams(float roomSize_, float damping_, float wet_, float dry_)
{
    roomSize = std::max(0.0f, std::min(roomSize_, 0.99f));
    damping = std::max(0.0f, std::min(damping_, 0.99f));
    wet = std::max(0.0f, std::min(wet_, 1.0f));
    dry = std::max(0.0f, std::min(dry_, 1.0f));

    // feedback gain for combs derived from room size
    // typical mapping: roomSize 0..1 -> feedback 0.7..0.98 (tunable)
    float feedbackBase = 0.7f + roomSize * 0.28f; // in [0.7, 0.98]
    for (size_t i = 0; i < combs.size(); ++i) {
        combs[i].setParams(feedbackBase, damping);
    }

    // allpass feedback typical ~0.5..0.6
    for (auto& ap : allpasses) ap.setFeedback(0.5f);
}

// process single sample (mono)
float SchroederReverb::processSample(float in)
{
    // feed input to each parallel comb
    float combSum = 0.0f;
    for (auto& c : combs) combSum += c.process(in);

    // average comb outputs
    float out = combSum * (1.0f / static_cast<float>(combs.size()));

    // serial allpass chain
    for (auto& ap : allpasses) out = ap.process(out);

    // mix wet/dry
    return (dry * in + wet * out);
}

// process a whole buffer in-place or to output buffer
void SchroederReverb::processBuffer(const float* inBuf, float* outBuf, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        outBuf[i] = processSample(inBuf[i]);
    }
}

// allow resetting internal state
void SchroederReverb::reset()
{
    initBuffers();
}

void SchroederReverb::initBuffers()
{
    // initialize combs and allpasses with sample-size buffers
    for (size_t i = 0; i < combs.size(); ++i) {
        size_t samples = msToSamples(combDelaysMs[i], sr);
        // ensure at least 1
        samples = std::max<size_t>(1, samples);
        combs[i].init(samples);
    }
    for (size_t i = 0; i < allpasses.size(); ++i) {
        size_t samples = msToSamples(allpassDelaysMs[i], sr);
        samples = std::max<size_t>(1, samples);
        allpasses[i].init(samples);
    }
}

size_t SchroederReverb::msToSamples(float ms, int sampleRate)
{
    return static_cast<size_t>(std::round(ms * sampleRate / 1000.0f));
}