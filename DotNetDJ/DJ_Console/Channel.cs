using MathNet.Numerics;
using NAudio.Wave;
using nl.AudioFilter;
using System;
using System.Collections.Generic;

namespace nl
{
    public class Channel : ISampleProvider
    {
        public WaveFormat WaveFormat => source.WaveFormat;

        public AudioFileReader source;

        private List<IAudioFilter> _filters;

        public VOLUME vol;
        public LSF lsf;
        public PF pf;
        public HSF hsf;

        public Channel()
        {
            _filters = new List<IAudioFilter>(8);

            // 먼저 추가된 순서대로 필터 적용
            _filters.Add(vol = new VOLUME(0.5f));
            _filters.Add(lsf = new LSF());
            _filters.Add(pf = new PF());
            _filters.Add(hsf = new HSF());
        }

        public int Read(float[] buffer, int offset, int count)
        {
            int n = 0;

            if (source != null)
                n = source.Read(buffer, offset, count);

            for (int i = 0; i < n; ++i)
            {
                for (int j = 0; j < _filters.Count; ++j)
                {
                    buffer[i] = _filters[j].Process(buffer[i]);
                }
            }

            return n;
        }
        
        public void SetSource(AudioFileReader source)
        {
            if (this.source != null)
            {
                this.source.Dispose();
            }

            for (int i = 0; i < _filters.Count; ++i)
            {
                _filters[i].OnSourceChanged(source);
            }

            this.source = source;
        }

        public void SetVolume(Byte analogValue, float crossFader)
        {
            float w = (float)analogValue / 255.0f;

            vol.Volume = Math.Clamp(w * crossFader, 0.0f, 1.0f);
        }

        public void SetEqHigh(Byte analogValue)
        {
            float w = (float)analogValue / 255.0f;

            hsf.Gain = GetGain(w);
            hsf.CutoffHz = GetFrequency(w, 20.0f, 2000.0f);
        }

        public void SetEqMid(Byte analogValue)
        {
            float w = (float)analogValue / 255.0f;

            pf.Gain = GetGain(w);
        }

        public void SetEqLow(Byte analogValue)
        {
            float w = (float)analogValue / 255.0f;

            lsf.Gain = GetGain(w);
            lsf.CutoffHz = GetFrequency(w, 2000.0f, 20000.0f);
        }

        public void SetWheel(Int32 dWheel, bool shift = false)
        {
            if (source == null)
                return;

            double milliseconds = (double)dWheel;

            // 정밀 조정, 샘플 단위 이동
            if (shift)
            {
                double r = (double)source.WaveFormat.SampleRate;
                milliseconds = 1000.0 / r;
            }

            TimeSpan newTime = source.CurrentTime + TimeSpan.FromMilliseconds(milliseconds);

            if (newTime < TimeSpan.Zero)
                newTime = TimeSpan.Zero;
            else if (newTime > source.TotalTime)
                newTime = source.TotalTime;

            source.CurrentTime = newTime;
        }

        private float GetGain(float w)
        {
            w = float.Clamp(2.0f * w, 1.0f / 1024.0f, 2.0f);

            // -60dB to +6dB
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
