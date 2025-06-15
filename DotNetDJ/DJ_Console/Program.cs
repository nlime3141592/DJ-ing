using NAudio.CoreAudioApi;
using NAudio.Wave;
using System;
using System.IO.Ports;
using System.Reflection.PortableExecutable;
using System.Threading;

namespace nl
{
    internal class Program
    {
        const int BlockSize = 1024;

        static void Main(string[] args)
        {
            //Console.WriteLine("WAV 파일 경로를 입력하세요:");
            //string path = Console.ReadLine();

            using AudioFileReader reader = new AudioFileReader(@"C:\Test\habibi.wav");

            var waveFormat = reader.WaveFormat;
            var waveOut = new WaveOutEvent();
            var bufferedWaveProvider = new BufferedWaveProvider(waveFormat)
            {
                BufferLength = BlockSize * 4,
                DiscardOnBufferOverflow = true
            };

            byte[] buffer = new byte[] { 0 };

            ArduinoIO io = new ArduinoIO();
            io.Open("COM10");

            io.Update();
            waveOut.Init(reader);
            waveOut.Play();

            while (io.Update() > 0)
            {
                int q5 = io.Controller.EqLow1;
                float v = (float)q5 / 255.0f;

                waveOut.Volume = v;

                Console.WriteLine($"Q5 == {q5}, v == {v}");
            }
            
            Console.WriteLine("재생 완료. Enter 키를 누르면 종료합니다.");
            Console.ReadLine();
        }

        private static void Main3(string[] args)
        {
            //AudioFileReader reader = new AudioFileReader(@"C:\Test\habibi.wav");

            //var fir = new double[257]; // 예: 저역통과 FIR
            //for (int i = 0; i < fir.Length; i++)
            //    fir[i] = Math.Sin(2 * Math.PI * 0.1 * i) / (Math.PI * i + 1e-9);

            //var filter = new FFTFilterOLA(fir);

            //// .wav 파일에서 블록 단위 읽고 처리
            //float[] inputBlock = new float[1024];
            //float[] outputBlock;

            //while (reader.Position < reader.Length)
            //{
            //    int read = reader.Read(inputBlock, 0, inputBlock.Length);
            //    if (read == 0) break;

            //    outputBlock = filter.Process(inputBlock);

            //    // PCM 변환 + 출력
            //}
        }

        private static void Main2(string[] args)
        {
            //FFTFilterDemo demo = new FFTFilterDemo();
            //demo.Start();

            Console.WriteLine("Press Enter to exit...");

            Console.ReadLine();
        }

        private static int Main1(string[] args)
        {
            // Get Serial Port
            int timeout = 5;
            string portName = null;

            while (timeout > 0)
            {
                //portName = ArduinoIO.InputGettingPortNameOrNull();

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
            //ArduinoIO io = new ArduinoIO(portName);

            // Update Audio
            //string audioPath1 = "C:\\Test\\bangalore.wav";
            //string audioPath2 = "C:\\Test\\habibi.wav";

            Controller controller = new Controller();

            //io.Open();
            //io.Update();

            controller.device.Init(controller);
            controller.device.Play();

            //controller.c1.ApplyWeights(io.W0, io.W1, io.W2, io.W3);
            //controller.c1.ApplyWeights(0.25f, io.W1, io.W2, io.W3);
            //controller.c2.ApplyWeights(io.W4, io.W5, io.W6, io.W7);
            //controller.c2.ApplyWeights(0.25f, io.W5, io.W6, io.W7);

            Console.WriteLine("Stat::");

            //while (!io.ExitFlag)
            //{
            //    io.Update();

            //    controller.c1.ApplyWeights(io.W0, io.W1, io.W2, io.W3);
            //    controller.c2.ApplyWeights(io.W4, io.W5, io.W6, io.W7);

            //    Console.WriteLine($"Position == {controller.device.GetPosition()}");

            //    Console.Write($"\r  W[] == {io.W0:F4}, {io.W1:F4}, {io.W2:F4}, {io.W3:F4}, {io.W4:F4}, {io.W5:F4}, {io.W6:F4}, {io.W7:F4}");
            //}

            controller.Dispose();

            //io.Close();

            return 0;
        }
    }
}
