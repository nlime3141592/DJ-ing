using NAudio.Wave;
using System;

namespace nl.AudioFilter
{
    public class VOLUME : IAudioFilter
    {
        // if true, then, mute volume.
        public bool Bypass { get; set; } = false;

        public float Volume
        {
            get => _b0;
            set => _b0 = value;
        }

        #region Filter Coefficients
        private float _b0;
        #endregion

        public VOLUME(float volume)
        {
            _b0 = volume;
        }

        public float Process(float sample)
        {
            if (Bypass)
                return 0.0f;

            return sample * _b0;
        }

        public void OnSourceChanged(ISampleProvider source)
        {
            // This block is intentionally no operation.
        }
    }
}
