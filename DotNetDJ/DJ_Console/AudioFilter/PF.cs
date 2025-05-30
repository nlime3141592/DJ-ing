using NAudio.Wave;
using System;

namespace nl.AudioFilter
{
    public class PF : AudioFilter
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

        public float Gain
        {
            get => _gain;
            set
            {
                if (_gain == value)
                    return;

                _gain = value;

                CalcCoefficients();
            }
        }

        public float QFactor
        {
            get => _qFactor;
            set
            {
                if (_qFactor == value)
                    return;

                _qFactor = value;

                CalcCoefficients();
            }
        }

        #region Filter Coefficients
        private float _a1, _a2;
        private float _b0, _b1, _b2;
        #endregion

        #region Feedback Terms
        private float _x2, _x1;
        private float _y2, _y1;
        #endregion

        private float _cutoffHz;
        private float _gain;
        private float _qFactor;

        public PF(ISampleProvider source, float cutoffHz = 1000.0f, float gain = 0.0f, float qFactor = 1.0f) : base(source)
        {
            _x2 = 0.0f;
            _x1 = 0.0f;

            _y2 = 0.0f;
            _y1 = 0.0f;

            _cutoffHz = cutoffHz;
            _gain = gain;
            _qFactor = qFactor;

            CalcCoefficients();
        }

        public override float Process(float sample)
        {
            float y = _b0 * sample + _b1 * _x1 + _b2 * _x2 - _a1 * _y1 - _a2 * _y2;

            _x2 = _x1;
            _x1 = sample;

            _y2 = _y1;
            _y1 = y;

            return y;
        }

        private void CalcCoefficients()
        {
            float w = 2.0f * MathF.PI * _cutoffHz / base.sampleRate;
            float a = MathF.Pow(10.0f, _gain / 40.0f);
            float cos = MathF.Cos(w);
            float sin = MathF.Sin(w);
            float h = sin / (2.0f * _qFactor);

            float a0 = 1.0f + h / a;
            _a1 = -2.0f * cos;
            _a2 = 1.0f - h / a;

            _b0 = 1.0f + h / a;
            _b1 = -2.0f * cos;
            _b2 = 1.0f - h / a;

            _a1 /= a0;
            _a2 /= a0;
            _b0 /= a0;
            _b1 /= a0;
            _b2 /= a0;
        }
    }
}
