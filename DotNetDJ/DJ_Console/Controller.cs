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

        public Controller()
        {
            device = new WaveOutEvent();

            c1 = new Channel("C:\\Test\\bangalore.wav");
            c2 = new Channel("C:\\Test\\habibi.wav");

            device.Volume = 1.0f; // Master Volume
            device.DesiredLatency = 300;
            
        }

        public void Dispose()
        {
            c1?.Dispose();
            c2?.Dispose();

            device?.Dispose();
        }

        public int Read(float[] buffer, int offset, int count)
        {
            float[] tBuffer = new float[count];

            int rd1 = c1.Read(buffer, offset, count);
            int rd2 = c2.Read(tBuffer, offset, count);

            int n = Math.Max(rd1, rd2);

            for (int i = 0; i < n; ++ i)
            {
                int j = offset + i;
                float mixed = buffer[j] + tBuffer[j];

                buffer[i] = Math.Clamp(mixed, -1.0f, 1.0f);
            }

            return n;
        }
    }
}
