using NAudio.Wave;
using System;
using System.Diagnostics;

namespace nl.AudioFilter
{
    public abstract class AudioFilter : IAudioFilter
    {
        public WaveFormat WaveFormat => source.WaveFormat;

        public bool Bypass { get; set; }
        public ISampleProvider source
        {
            get => _source;
            set
            {
                _source = value;
                sampleRate = _source.WaveFormat.SampleRate;
            }
        }

        protected int sampleRate { get; private set; }

        private readonly object _lock;
        private ISampleProvider _source;

        public AudioFilter(ISampleProvider source)
        {
            //Debug.Assert(source != null, "Audio source cannot be null.");

            Bypass = false;

            _lock = new object();
            _source = source;
            sampleRate = _source.WaveFormat.SampleRate;
        }

        public abstract float Process(float sample);

        public int Read(float[] buffer, int offset, int count)
        {
            int num = source.Read(buffer, offset, count);

            lock (_lock)
            {
                for (int i = 0; i < num; ++i)
                {
                    buffer[i] = this.Process(buffer[i]);
                }
            }

            return num;
        }
    }
}
