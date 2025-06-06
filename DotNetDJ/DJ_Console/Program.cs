using NAudio.CoreAudioApi;
using NAudio.Wave;
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
            //string audioPath1 = "C:\\Test\\bangalore.wav";
            //string audioPath2 = "C:\\Test\\habibi.wav";

            Controller controller = new Controller();

            io.Open();
            //io.Update();

            controller.device.Init(controller);
            controller.device.Play();

            //controller.c1.ApplyWeights(io.W0, io.W1, io.W2, io.W3);
            controller.c1.ApplyWeights(0.25f, io.W1, io.W2, io.W3);
            //controller.c2.ApplyWeights(io.W4, io.W5, io.W6, io.W7);
            controller.c2.ApplyWeights(0.25f, io.W5, io.W6, io.W7);

            Console.WriteLine("Stat::");

            while (!io.ExitFlag)
            {
                //io.Update();

                //controller.c1.ApplyWeights(io.W0, io.W1, io.W2, io.W3);
                //controller.c2.ApplyWeights(io.W4, io.W5, io.W6, io.W7);

                Console.WriteLine($"Position == {controller.device.GetPosition()}");

                //Console.Write($"\r  W[] == {io.W0:F4}, {io.W1:F4}, {io.W2:F4}, {io.W3:F4}, {io.W4:F4}, {io.W5:F4}, {io.W6:F4}, {io.W7:F4}");
            }

            controller.Dispose();

            io.Close();

            return 0;
        }
    }
}
