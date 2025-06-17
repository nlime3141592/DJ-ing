using System;

namespace nl.ConsoleCommand
{
    public abstract class Command
    {
        public const int c_HALT = -1;
        public const int c_FAIL = 0;
        public const int c_SUCCESS = 1;

        public abstract int Switch();

        public static int Read()
        {
            int command = 0;

            if (!int.TryParse(Console.ReadLine(), out command))
                return Command.c_FAIL;

            return command;
        }
    }
}
