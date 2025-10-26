#include "audioreverb.h"

SchroederReverb_Comb::SchroederReverb_Comb() : 
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

void SchroederReverb_Comb::ClearBuffer()
{
    lpfBuffer = 0.0f;
    std::fill(xBuffer, xBuffer + BUFFER_CAPACITY, 0.0f);
    std::fill(yBuffer, yBuffer + BUFFER_CAPACITY, 0.0f);
}

void SchroederReverb_Comb::SetDelay(float delay_ms, int32_t sampleRate)
{
    this->sampleRate = sampleRate;
    this->delay_ms = delay_ms;
    this->delay_sample = (int32_t)((float)sampleRate * delay_ms / 1000.0f);
}

void SchroederReverb_Comb::SetFeedback(float feedback)
{
    assert(feedback >= 0.0f && feedback <= 1.0f);

    float feedbackInternal = 0.99f * feedback;
    this->feedback = feedbackInternal;
}

void SchroederReverb_Comb::SetDamping(float damping)
{
    assert(damping >= 0.0f && damping <= 1.0f);

    float dampingInternal = 0.01f + 0.98f * damping;
    this->damping = dampingInternal;
    this->dampingInverse = 1.0f - dampingInternal;
}

float SchroederReverb_Comb::Process(float input)
{
    int32_t delay = delay_sample;
    int32_t index_1 = index < 1 ? BUFFER_CAPACITY - 1 : index - 1;
    int32_t index_d = (index - delay + BUFFER_CAPACITY) % BUFFER_CAPACITY;

    xBuffer[index] = input;

    // 1차 LPF
    float output_lpf = yBuffer[index_d];
    lpfBuffer = (dampingInverse * output_lpf) + (damping * output_lpf);

    // 1차 Comb Filter
    float output_comb = xBuffer[index] + feedback * lpfBuffer;

    yBuffer[index] = output_comb;

    index = (index + 1) % BUFFER_CAPACITY;

    return output_comb;
}

SchroederReverb_Allpass::SchroederReverb_Allpass() : 
    index(0),
    feedback(0.5f)
{
    ClearBuffer();
}

void SchroederReverb_Allpass::ClearBuffer()
{
    std::fill(xBuffer, xBuffer + BUFFER_CAPACITY, 0.0f);
    std::fill(yBuffer, yBuffer + BUFFER_CAPACITY, 0.0f);
}

void SchroederReverb_Allpass::SetDelay(float delay_ms, int32_t sampleRate)
{
    this->sampleRate = sampleRate;
    this->delay_ms = delay_ms;
    this->delay_sample = (int32_t)((float)sampleRate * delay_ms / 1000.0f);
}

void SchroederReverb_Allpass::SetFeedback(float feedback)
{
    assert(feedback >= 0.0f && feedback <= 1.0f);

    this->feedback = feedback;
}

float SchroederReverb_Allpass::Process(float input)
{
    int32_t delay = delay_sample;
    int32_t index_1 = index < 1 ? BUFFER_CAPACITY - 1 : index - 1;
    int32_t index_d = (index - delay + BUFFER_CAPACITY) % BUFFER_CAPACITY;

    xBuffer[index] = input;

    // 1차 APF
    float output = feedback * (yBuffer[index_d] - xBuffer[index_d]) + xBuffer[index];

    yBuffer[index] = output;

    index = (index + 1) % BUFFER_CAPACITY;

    return output;
}

int16_t SchroederReverb::Process(int16_t input)
{
    float input01 = static_cast<float>(input) / 32768.0f;
    float output01 = Process(input01);

    return static_cast<int16_t>(std::max(-32768.0f, std::min(output01, 32767.0f)));
}

SchroederReverb::SchroederReverb(int sampleRate) :
    roomSize(0.2f),
    damping(0.2f),
    wet(0.33f),
    dry(0.67f)
{
    combDelaysMs = { 32.4f, 33.7f, 31.1f, 29.6f };
    allpassDelaysMs = { 4.7f, 0.5f };

    combs.resize(combDelaysMs.size());
    allpasses.resize(allpassDelaysMs.size());

    for (size_t i = 0; i < combs.size(); ++i)
    {
        combs[i].SetDelay(combDelaysMs[i], 44100);

        combs[i].SetFeedback(roomSize);
        combs[i].SetDamping(damping);
    }

    for (size_t i = 0; i < allpasses.size(); ++i)
    {
        allpasses[i].SetDelay(allpassDelaysMs[i], 44100);

        allpasses[i].SetFeedback(roomSize);
    }
}

void SchroederReverb::SetRoomSize(float roomSize)
{
    assert(roomSize >= 0.0f && roomSize <= 1.0f);

    this->roomSize = 0.7f + 0.28f * roomSize;
    //this->roomSize = roomSize;
    
    for (size_t i = 0; i < combs.size(); ++i)
    {
        combs[i].SetFeedback(roomSize);
    }

    for (size_t i = 0; i < allpasses.size(); ++i)
    {
        allpasses[i].SetFeedback(roomSize);
    }
}

void SchroederReverb::SetDamping(float damping)
{
    assert(damping >= 0.0f && damping <= 1.0f);

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

float SchroederReverb::Process(float input)
{
    float combSum = 0.0f;

    // Comb Filter의 병렬 연결
    for (size_t i = 0; i < combs.size(); ++i)
    {
        combSum += combs[i].Process(input);
    }

    float output = combSum * (1.0f / static_cast<float>(combs.size()));

    // APF의 직렬 연결
    for (size_t i = 0; i < allpasses.size(); ++i)
    {
        output = allpasses[i].Process(output);
    }

    return (dry * input + wet * output);
}

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