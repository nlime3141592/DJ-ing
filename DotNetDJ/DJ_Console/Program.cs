using nl.ConsoleCommand;
using System;
using System.Threading;

namespace nl
{
    internal class Program
    {
        public static Controller controller;
        public static ArduinoIO io;

        private static void InputThread(object portName)
        {
            io = new ArduinoIO();

            io.Open((string)portName);

            while (io.Update() > 0)
            {
                controller.Update(io);
                Console.WriteLine($"{io.Deck1.BtnPlay}, {io.Deck2.BtnPlay}");
            }
        }


        private static string SelectPort()
        {
            string[] ports = ArduinoIO.GetPortNames();

            int index;

            for (int i = 0; i < ports.Length; ++i)
            {
                Console.WriteLine($"{i + 1:D02}: {ports[i]}");
            }

            Console.Write("Select Port:: ");

            if (!int.TryParse(Console.ReadLine(), out index))
            {
                return string.Empty;
            }


            return ports[index - 1];
        }

        private static void Main(string[] args)
        {
            // init controller
            controller = new Controller();

            // init input
            ArduinoIO io = new ArduinoIO();
            string portName = SelectPort();

            if (portName == null)
            {
                return;
            }

            Thread ioThread = new Thread(new ParameterizedThreadStart(Program.InputThread));
            ioThread.Start((object)portName);

            RootCommand command = new RootCommand();

            while (command.Switch() != Command.c_HALT) ;

            ioThread.Interrupt();

            Console.WriteLine("Enter 키를 누르면 종료합니다.");
            Console.ReadLine();
        }
    }
}
