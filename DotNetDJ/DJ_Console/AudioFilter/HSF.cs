using NAudio.Wave;
using System;

namespace nl.AudioFilter
{
    public class HSF : AudioFilter
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

        public HSF(ISampleProvider source, float cutoffHz = 10000.0f, float gain = 0.0f, float qFactor = 1.0f) : base(source)
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

            // TODO: q를 slope로 변환하는 공식을 이 곳에 적용하세요.
            float slope = _qFactor;

            float h1 = sin / 2.0f * MathF.Sqrt((a + 1.0f / 1.0f) * (1.0f / slope - 1.0f) + 2.0f);
            float h2 = 2.0f * h1 * MathF.Sqrt(a);

            float a0 = (a + 1.0f) - (a - 1.0f) * cos + h2;
            _a1 = 2.0f * ((a - 1.0f) - (a + 1.0f) * cos);
            _a2 = (a + 1.0f) - (a - 1.0f) * cos - h2;

            _b0 = a * ((a + 1.0f) + (a - 1.0f) * cos + h2);
            _b1 = -2.0f * a * ((a - 1.0f) + (a + 1.0f) * cos);
            _b2 = a * ((a + 1.0f) + (a - 1.0f) * cos - h2);

            _a1 /= a0;
            _a2 /= a0;
            _b0 /= a0;
            _b1 /= a0;
            _b2 /= a0;
        }
    }
}
