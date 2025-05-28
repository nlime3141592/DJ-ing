using NAudio.Wave;
using System.Diagnostics;

namespace nl.AudioFilter
{
    public abstract class AudioFilter : IAudioFilter
    {
        public WaveFormat WaveFormat => source.WaveFormat;

        public bool Bypass { get; set; }

        protected readonly ISampleProvider source;
        protected readonly int sampleRate;

        private readonly object _lock;

        public AudioFilter(ISampleProvider source)
        {
            Debug.Assert(source != null, "Audio source cannot be null.");

            Bypass = false;

            this.source = source;
            sampleRate = source.WaveFormat.SampleRate;

            _lock = new object();
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
