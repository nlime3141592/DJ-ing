using NAudio.Wave;
using System;

using nl.AudioFilter;

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
            string audioPath1 = "C:\\Test\\tiger.wav";
            string audioPath2 = "C:\\Test\\breakaway.wav";

            Controller controller1 = new Controller(audioPath1);
            Controller controller2 = new Controller(audioPath2);

            controller1.device.Play();
            controller2.device.Play();

            io.Open();

            while (!io.ExitFlag)
            {
                io.Update();

                controller1.ApplyWeights(io.W0, io.W1, io.W2, io.W3);
                controller2.ApplyWeights(io.W4, io.W5, io.W6, io.W7);
            }

            controller1.Dispose();
            controller2.Dispose();
            io.Close();

            return 0;
        }
    }
}
