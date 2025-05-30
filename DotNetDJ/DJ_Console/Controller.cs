using NAudio.Wave;
using nl.AudioFilter;
using System;

namespace nl
{
    public class Controller
    {
        public LSF lsf { get; private set; }
        public PF pf { get; private set; }
        public HSF hsf { get; private set; }

        public WaveOutEvent device { get; private set; }

        private string _audioPath;
        private AudioFileReader _audioFile;

        public ISampleProvider source1 { get; set; }
        public ISampleProvider source2 { get; set; }

        public Controller(string audioPath)
        {
            _audioPath = audioPath;
            _audioFile = new AudioFileReader(audioPath);

            device = new WaveOutEvent();

            lsf = new LSF(_audioFile);
            pf = new PF(lsf);
            hsf = new HSF(pf);

            device.DesiredLatency = 300;
            device.Init(hsf);
        }

        public void ApplyWeights(float w0, float w1, float w2, float w3)
        {
            device.Volume = w0;

            lsf.Gain = GetGain(w1);
            lsf.CutoffHz = GetFrequency(w1, 20.0f, 2000.0f);

            pf.Gain = GetGain(w2);

            hsf.Gain = GetGain(w3);
            hsf.CutoffHz = GetFrequency(w3, 2000.0f, 20000.0f);

            Console.WriteLine("Stat::");
            Console.WriteLine($"  W[] == {w0}, {w1}, {w2}, {w3}");
            Console.WriteLine($"  G[] == {lsf.Gain}, {pf.Gain}, {hsf.Gain}");
            Console.WriteLine($"  F[] == {lsf.CutoffHz}, {pf.CutoffHz}, {hsf.CutoffHz}");
        }

        public void Dispose()
        {
            device?.Dispose();
            _audioFile?.Dispose();
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

        public int Read(float[] buffer, int offset, int count)
        {
            int num1 = source1.Read(buffer, offset, count);
            int num2 = source2.Read(buffer, offset, count);

            return count;
        }
    }
}
