using NAudio.Wave;
using System;

namespace nl
{
    internal class Program
    {
        static void Main(string[] args)
        {
            // set audio file
            string path1 = "C:\\Test\\bangalore.wav";
            string path2 = "C:\\Test\\habibi.wav";

            AudioFileReader audio1 = new AudioFileReader(path1);
            AudioFileReader audio2 = new AudioFileReader(path2);

            // init controller
            Controller controller = new Controller();
            controller.c1.SetSource(audio1);
            controller.c2.SetSource(audio2);

            // init input
            ArduinoIO io = new ArduinoIO();
            string[] ports = ArduinoIO.GetPortNames();
            int portIndex;

            for (int i = 0; i < ports.Length; ++i)
            {
                Console.WriteLine($"{i + 1}: {ports[i]}");
            }

            Console.Write("Select Port:: ");

            if (!int.TryParse(Console.ReadLine(), out portIndex))
            {
                Console.WriteLine("Port Number Error. Exit Program.");
                return;
            }

            io.Open(ports[portIndex - 1]);
            io.Update();

            // init device
            controller.device = new WaveOutEvent();
            controller.device.Init(controller);
            controller.device.Volume = 1.0f;
            controller.device.Play();

            //controller.isPlay2 = true;

            while (io.Update() > 0)
            {
                controller.Update(io);

                //byte vf0 = io.Controller.VolumeFader0;
                //byte vf1 = io.Controller.VolumeFader1;
                //Console.WriteLine($"vf0 == {vf0}, vf1 == {vf1}");

                Console.WriteLine($"dWheel1 == {io.Deck1.DeltaWheel}, CurrentTime == {controller.c1.source.CurrentTime.TotalMilliseconds}");
            }
            
            Console.WriteLine("재생 완료. Enter 키를 누르면 종료합니다.");
            Console.ReadLine();
        }
    }
}
