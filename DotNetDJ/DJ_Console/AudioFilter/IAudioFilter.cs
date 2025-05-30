using NAudio.Wave;

namespace nl.AudioFilter
{
    public interface IAudioFilter
    {
        bool Bypass { get; set; }

        float Process(float sample);
        void OnSourceChanged(ISampleProvider source);
    }
}
