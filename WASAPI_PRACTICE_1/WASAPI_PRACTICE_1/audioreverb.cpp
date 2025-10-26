#include "audioreverb.h"

Comb::Comb() : 
    lpfBuffer(0.0f),
    index(0),
    sampleRate(44100.0f),
    feedback(0.0f),
    damping(0.2f),
    dampingInverse(0.8f)
{
    // NOTE: 인덱스가 버퍼를 순환하기 때문에, 인덱스 값을 굳이 0으로 초기화시켜 줄 필요가 없음.
    ClearBuffer();
}

void Comb::ClearBuffer()
{
    lpfBuffer = 0.0f;
    std::fill(xBuffer, xBuffer + BUFFER_CAPACITY, 0.0f);
    std::fill(yBuffer, yBuffer + BUFFER_CAPACITY, 0.0f);
}

void Comb::SetDelay(float delay_ms, int32_t sampleRate)
{
    this->sampleRate = sampleRate;
    this->delay_ms = delay_ms;
    this->delay_sample = (int32_t)((float)sampleRate * delay_ms / 1000.0f);
}

void Comb::SetFeedback(float feedback)
{
    assert(feedback >= 0.0f && feedback <= 0.99f);

    this->feedback = feedback;
}

void Comb::SetDamping(float damping)
{
    assert(damping >= 0.01f && damping <= 0.99f);

    this->damping = damping;
    this->dampingInverse = 1.0f - damping;
}

float Comb::Process(float input)
{
    int32_t delay = delay_sample;
    int32_t index_1 = index < 1 ? BUFFER_CAPACITY - 1 : index - 1;
    int32_t index_d = (index - delay + BUFFER_CAPACITY) % BUFFER_CAPACITY;

    xBuffer[index] = input;

    float output_lpf = yBuffer[index_d];
    lpfBuffer = (dampingInverse * output_lpf) + (damping * output_lpf);
    float output_comb = xBuffer[index] + feedback * lpfBuffer;

    //float output_lpf = (dampingInverse * xBuffer[index_d]) + (damping * yBuffer[index_1]);
    //float output_comb = xBuffer[index] + feedback * yBuffer[index_d];

    yBuffer[index] = output_comb;

    index = (index + 1) % BUFFER_CAPACITY;

    return output_comb;
}

Allpass::Allpass() : 
    index(0),
    feedback(0.5f)
{
    ClearBuffer();
}

void Allpass::ClearBuffer()
{
    std::fill(xBuffer, xBuffer + BUFFER_CAPACITY, 0.0f);
    std::fill(yBuffer, yBuffer + BUFFER_CAPACITY, 0.0f);
}

void Allpass::SetDelay(float delay_ms, int32_t sampleRate)
{
    this->sampleRate = sampleRate;
    this->delay_ms = delay_ms;
    this->delay_sample = (int32_t)((float)sampleRate * delay_ms / 1000.0f);
}

void Allpass::SetFeedback(float feedback)
{
    this->feedback = feedback;
}

float Allpass::Process(float input)
{
    int32_t delay = delay_sample;
    int32_t index_1 = index < 1 ? BUFFER_CAPACITY - 1 : index - 1;
    int32_t index_d = (index - delay + BUFFER_CAPACITY) % BUFFER_CAPACITY;

    xBuffer[index] = input;

    float output = feedback * (yBuffer[index_d] - xBuffer[index]) + xBuffer[index_d];

    yBuffer[index] = output;

    index = (index + 1) % BUFFER_CAPACITY;

    return output;
}

SchroederReverb::SchroederReverb(int sampleRate) :
    sr(sampleRate),
    roomSize(0.2f),
    damping(0.2f),
    wet(0.33f),
    dry(0.67f)
{
    combDelaysMs = { 32.4f, 33.7f, 31.1f, 29.6f };
    allpassDelaysMs = { 4.7f, 0.5f };

    combs.resize(combDelaysMs.size());
    allpasses.resize(allpassDelaysMs.size());

    float roomSizeInternal = GetRoomSizeInternal(this->roomSize);

    for (size_t i = 0; i < combs.size(); ++i)
    {
        combs[i].SetDelay(combDelaysMs[i], 44100);

        combs[i].SetFeedback(roomSizeInternal);
        combs[i].SetDamping(damping);
    }

    for (size_t i = 0; i < allpasses.size(); ++i)
    {
        allpasses[i].SetDelay(allpassDelaysMs[i], 44100);

        allpasses[i].SetFeedback(roomSizeInternal);
    }
}

void SchroederReverb::SetRoomSize(float roomSize)
{
    assert(roomSize >= 0.0f && roomSize <= 1.0f);

    this->roomSize = roomSize;
    float roomSizeInternal = GetRoomSizeInternal(roomSize);

    for (size_t i = 0; i < combs.size(); ++i)
    {
        combs[i].SetFeedback(roomSizeInternal);
    }

    for (size_t i = 0; i < allpasses.size(); ++i)
    {
        allpasses[i].SetFeedback(roomSize);
    }
}

void SchroederReverb::SetDamping(float damping)
{
    assert(damping >= 0.01f && damping <= 0.99f);

    this->damping = damping;

    for (size_t i = 0; i < combs.size(); ++i)
    {
        combs[i].SetDamping(damping);
    }
}

void SchroederReverb::SetWet(float wet)
{
    assert(wet >= 0.0f && wet <= 1.0f);

    this->wet = wet;
    this->dry = 1.0f - wet;
}

// process single sample (mono)
float SchroederReverb::Process(float input)
{
    float combSum = 0.0f;

    for (size_t i = 0; i < combs.size(); ++i)
    {
        combSum += combs[i].Process(input);
    }

    float output = combSum * (1.0f / static_cast<float>(combs.size()));

    for (size_t i = 0; i < allpasses.size(); ++i)
    {
        output = allpasses[i].Process(output);
    }

    return (dry * input + wet * output);
}

// process a whole buffer in-place or to output buffer
void SchroederReverb::ProcessBuffer(const float* inBuf, float* outBuf, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        outBuf[i] = Process(inBuf[i]);
    }
}

void SchroederReverb::ClearBuffer()
{
    for (size_t i = 0; i < combs.size(); ++i)
    {
        combs[i].ClearBuffer();
    }

    for (size_t i = 0; i < allpasses.size(); ++i)
    {
        allpasses[i].ClearBuffer();
    }
}

float SchroederReverb::GetRoomSizeInternal(float roomSizeExternal)
{
    assert(roomSizeExternal >= 0.0f && roomSizeExternal <= 1.0f);

    return 0.0f + 0.98f * roomSizeExternal;
}