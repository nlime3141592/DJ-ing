using NAudio.Wave;

namespace nl.AudioFilter
{
    public interface IAudioFilter : ISampleProvider
    {
        bool Bypass { get; set; }

        float Process(float sample);
    }
}
