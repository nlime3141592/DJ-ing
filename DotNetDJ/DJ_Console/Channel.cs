using NAudio.CoreAudioApi;
using NAudio.Wave;
using nl.AudioFilter;
using System;

namespace nl
{
    public class Channel : ISampleProvider
    {
        public WaveFormat WaveFormat => source.WaveFormat;

        public AudioFileReader source;

        public float volume;
        public LSF lsf;
        public PF pf;
        public HSF hsf;

        public Channel(string audioPath)
        {
            source = new AudioFileReader(audioPath);

            volume = 0.5f;
            lsf = new LSF();
            pf = new PF();
            hsf = new HSF();

            lsf.OnSourceChanged(source);
            pf.OnSourceChanged(source);
            hsf.OnSourceChanged(source);
        }

        public int Read(float[] buffer, int offset, int count)
        {
            int n = source.Read(buffer, offset, count);

            for (int i = 0; i < n; ++i)
            {
                float sample = buffer[i];

                sample = lsf.Process(sample);
                sample = pf.Process(sample);
                sample = hsf.Process(sample);
                sample *= volume;

                buffer[i] = sample;
            }

            return n;
        }

        public void ApplyWeights(float w0, float w1, float w2, float w3)
        {
            volume = w0;

            lsf.Gain = GetGain(w1);
            lsf.CutoffHz = GetFrequency(w1, 20.0f, 2000.0f);

            pf.Gain = GetGain(w2);

            hsf.Gain = GetGain(w3);
            hsf.CutoffHz = GetFrequency(w3, 2000.0f, 20000.0f);

            //Console.WriteLine("Stat::");
            //Console.WriteLine($"  W[] == {w0}, {w1}, {w2}, {w3}");
            //Console.WriteLine($"  G[] == {lsf.Gain}, {pf.Gain}, {hsf.Gain}");
            //Console.WriteLine($"  F[] == {lsf.CutoffHz}, {pf.CutoffHz}, {hsf.CutoffHz}");
        }

        public void Dispose()
        {
            source?.Dispose();
        }

        private float GetGain(float w)
        {
            w = float.Clamp(2.0f * w, 1.0f / 1024.0f, 2.0f);
            return MathF.Log(w, MathF.Pow(2.0f, 1.0f / 6.0f));
        }

        private float GetFrequency(float w, float min, float max)
        {
            if (w < 0.5f)
                return float.Lerp(max, min, 2.0f * w);
            else
                return float.Lerp(min, max, 2.0f * (w - 0.5f));
        }
    }
}
