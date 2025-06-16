using NAudio.Wave;
using System;

namespace nl
{
    public class Controller : ISampleProvider
    {
        public WaveFormat WaveFormat => c1.WaveFormat;

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
            device = new WaveOutEvent();

            c1 = new Channel();
            c2 = new Channel();

            device.Volume = 1.0f; // Master Volume
            device.DesiredLatency = 300;
        }

        public void Dispose()
        {
            //c1?.Dispose();
            //c2?.Dispose();

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
            if (!isPlay1 && !isPlay2)
                device.Stop();
            else if (device.PlaybackState != PlaybackState.Playing)
                device.Play();
        }

        public int Read(float[] buffer, int offset, int count)
        {
            float[] tBuffer = new float[count];
            int rdLength1 = 0;
            int rdLength2 = 0;

            if (isPlay1)
                rdLength1 = c1.Read(buffer, offset, count);

            if (isPlay2)
                rdLength2 = c2.Read(tBuffer, offset, count);

            int n = Math.Max(rdLength1, rdLength2);

            for (int i = 0; i < n; ++i)
            {
                int j = offset + i;
                
                buffer[i] = Math.Clamp(buffer[j] + tBuffer[j], -1.0f, 1.0f);
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
