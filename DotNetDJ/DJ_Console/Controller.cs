using NAudio.Wave;
using System;

namespace nl
{
    public class Controller : ISampleProvider
    {
        public WaveFormat WaveFormat
        {
            get
            {
                if (c1 == null)
                    return c2.WaveFormat;
                else
                    return c1.WaveFormat;
            }
        }

        public WaveOutEvent device;

        public Channel c1;
        public Channel c2;

        private bool isPlayPrev1;
        private bool isPlayPrev2;
        private bool isPlayNext1;
        private bool isPlayNext2;

        public bool isPlay1;
        public bool isPlay2;

        public Controller()
        {
            c1 = new Channel();
            c2 = new Channel();
        }

        public void SetSource(AudioFileReader source, int index)
        {
            switch (index)
            {
                case 1:
                    c1.SetSource(source);
                    break;
                case 2:
                    c2.SetSource(source);
                    break;
                default:
                    return;
            }

            if (device == null)
            {
                WaveOutEvent waveOut = new WaveOutEvent();
                waveOut.Init(this);
                device = waveOut;
            }
        }

        public void Dispose()
        {
            c1.source?.Dispose();
            c2.source?.Dispose();
            device?.Dispose();
        }

        public void Update(ArduinoIO io)
        {
            float xf1 = GetCrossFader1(io.Controller.CrossFader);
            float xf2 = GetCrossFader2(io.Controller.CrossFader);

            c1.SetVolume(io.Controller.VolumeFader0, xf1);
            c1.SetEqHigh(io.Controller.EqHigh0);
            c1.SetEqMid(io.Controller.EqMid0);
            c1.SetEqLow(io.Controller.EqLow0);
            c1.SetWheel(io.Deck1.DeltaWheel);

            c2.SetVolume(io.Controller.VolumeFader1, xf2);
            c2.SetEqHigh(io.Controller.EqHigh1);
            c2.SetEqMid(io.Controller.EqMid1);
            c2.SetEqLow(io.Controller.EqLow1);
            c2.SetWheel(io.Deck2.DeltaWheel);

            UpdatePlay(io.Deck1.BtnPlay, io.Deck2.BtnPlay);
        }

        private void UpdatePlay(bool isPlayDown1, bool isPlayDown2)
        {
            // button의 상승 엣지 검출
            isPlayPrev1 = isPlayNext1;
            isPlayPrev2 = isPlayNext2;

            isPlayNext1 = isPlayDown1;
            isPlayNext2 = isPlayDown2;

            bool triggered = false;

            if (triggered |= (!isPlayPrev1 && isPlayNext1))
                isPlay1 = !isPlay1;
            
            if (triggered |= (!isPlayPrev2 && isPlayNext2))
                isPlay2 = !isPlay2;

            if (!triggered)
                return;

            // play and stop 처리
            if (device == null)
                return;
            else if (!isPlay1 && !isPlay2)
                device.Stop();
            else if (device.PlaybackState != PlaybackState.Playing)
                device.Play();
        }

        public int Read(float[] buffer, int offset, int count)
        {
            float[] tBuffer1 = new float[count];
            float[] tBuffer2 = new float[count];

            int rdLength1 = 0;
            int rdLength2 = 0;

            if (isPlay1)
                rdLength1 = c1.Read(tBuffer1, offset, count);

            if (isPlay2)
                rdLength2 = c2.Read(tBuffer2, offset, count);

            int i = 0;
            int n = Math.Max(rdLength1, rdLength2);

            for (i = 0; i < n; ++i)
            {
                float sample1 = i < rdLength1 ? tBuffer1[i] : 0.0f;
                float sample2 = i < rdLength2 ? tBuffer2[i] : 0.0f;

                buffer[offset + i] = Math.Clamp(sample1 + sample2, -1.0f, 1.0f);
            }

            return n;
        }

        private float GetCrossFader1(Byte analogValue)
        {
            float w = (float)analogValue / 255.0f;
            w = 1.0f - w;

            if (w < 0.45f)
                w /= 0.45f;

            w = Math.Clamp(w, 0.0f, 1.0f);
            return w;
        }

        private float GetCrossFader2(Byte analogValue)
        {
            float w = (float)analogValue / 255.0f;

            if (w < 0.45f)
                w /= 0.45f;

            w = Math.Clamp(w, 0.0f, 1.0f);
            return w;
        }
    }
}
