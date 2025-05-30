using NAudio.Wave;
using System;
using System.Diagnostics;

namespace nl.AudioFilter
{
    /// <summary>
    /// Biquad Low Pass Filter
    /// </summary>
    public class BQLPF //: AudioFilter
    {
        public float CutoffHz
        {
            get => _cutoffHz;
            set
            {
                if (_cutoffHz == value)
                    return;

                _cutoffHz = value;

                CalcCoefficients();
            }
        }

        #region Filter Coefficients
        private float _a0, _a1, _a2;
        private float _b1, _b2;
        #endregion

        #region Feedback Terms
        private float _y2, _y1;
        #endregion

        private float _cutoffHz;

        public BQLPF(ISampleProvider source, float cutoffHz = 1000.0f)// : base(source)
        {
            _y2 = 0.0f;
            _y1 = 0.0f;

            _cutoffHz = cutoffHz;

            CalcCoefficients();
        }

        //public override float Process(float sample)
        public float Process(float sample)

        {
            float y = _a0 * sample + _a1 * _y1 + _a2 * _y2 - _b1 * _y1 - _b2 * _y2;
            _y2 = _y1;
            _y1 = y;
            return y;
        }

        private void CalcCoefficients()
        {
            //float w = 2.0f * MathF.PI * _cutoffHz / base.sampleRate;
            float w = 2.0f * MathF.PI * _cutoffHz / 44100.0f;
            float cos = MathF.Cos(w);
            float sin = MathF.Sin(w);
            float h = sin / MathF.Sqrt(2.0f);

            float b0 = (1.0f - cos) / 2.0f;
            float b1n = 1 - cos;
            float b2n = b0;
            float a0n = 1 + h;

            _a0 = b0 / a0n;
            _a1 = b1n / a0n;
            _a2 = b2n / a0n;
            _b1 = -2.0f * cos / a0n;
            _b2 = (1.0f - h) / a0n;
        }
    }
}
