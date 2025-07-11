﻿using NAudio.Wave;
using System;

namespace nl
{
    public class Controller : ISampleProvider
    {
        public WaveFormat WaveFormat
        {
            get
            {
                if (c1.source == null)
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
                waveOut.DesiredLatency = 500;
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

            if (!isPlayPrev1 && isPlayNext1)
            {
                triggered = true;
                isPlay1 = !isPlay1;
            }
                
            
            if (!isPlayPrev2 && isPlayNext2)
            {
                triggered = true;
                isPlay2 = !isPlay2;
            }

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

        // WaveOutEvent 클래스가 이 함수를 내부에서 호출함
        public int Read(float[] buffer, int offset, int count)
        {
            float[] tBuffer1 = new float[count];
            float[] tBuffer2 = new float[count];

            int rdLength1 = 0;
            int rdLength2 = 0;

            // 재생 상태에 따라서 샘플을 읽거나 읽지 않음
            if (isPlay1)
                rdLength1 = c1.Read(tBuffer1, offset, count);
            if (isPlay2)
                rdLength2 = c2.Read(tBuffer2, offset, count);

            int i = 0;
            int n = Math.Max(rdLength1, rdLength2);

            // 두 샘플을 더해서 출력함
            // 매개변수로 들어온 buffer 배열에 값을 쓰는 것으로 디바이스에 최종 출력하는 방식
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
