using NAudio.Wave;
using nl.AudioFilter;
using System;

namespace nl
{
    internal class Program
    {
        private static int Main(string[] args)
        {
            // Get Serial Port
            int timeout = 5;
            string portName = null;

            while (timeout > 0)
            {
                portName = ArduinoIO.InputGettingPortNameOrNull();

                if (portName != null)
                    break;

                --timeout;
            }

            if (portName == null || timeout == 0)
            {
                Console.WriteLine("Cannot find Serial Port");
                return -1;
            }

            // Setting Port
            ArduinoIO io = new ArduinoIO(portName);

            // Update Audio
            string audioPath = "C:\\Test\\bangalore.wav";

            float fMin = 20.0f;
            float fMax = 20000.0f;

            using (AudioFileReader audioFile = new AudioFileReader(audioPath))
            using (WaveOutEvent outputDevice = new WaveOutEvent())
            {
                BQLPF lpf = new BQLPF(audioFile);

                io.Open();
                io.Update();

                float f = float.Lerp(fMin, fMax, io.W0);
                lpf.CutoffHz = f;
                Console.WriteLine($"Cutoff Freq == {f}");

                outputDevice.Init(lpf);
                outputDevice.Play();

                while (!io.ExitFlag && outputDevice.PlaybackState == PlaybackState.Playing)
                {
                    io.Update();

                    f = float.Lerp(fMin, fMax, io.W0);
                    lpf.CutoffHz = f;
                    Console.WriteLine($"Cutoff Freq == {f}");
                }

                io.Close();
            }

            return 0;
        }
    }
}
