using System;

namespace nl.ConsoleCommand
{
    public class RootCommand : Command
    {
        private SelectMusicCommand _selectMusicCommand1;
        private SelectMusicCommand _selectMusicCommand2;
        private SelectEQ _eqCommand;

        public RootCommand()
        {
            _selectMusicCommand1 = new SelectMusicCommand(1);
            _selectMusicCommand2 = new SelectMusicCommand(2);
            _eqCommand = new SelectEQ();
        }

        public override int Switch()
        {
            Console.WriteLine("  0: Program Exit");
            Console.WriteLine("  1: Select Music on Channel #1");
            Console.WriteLine("  2: Select Music on Channel #2");
            Console.WriteLine("  3: Select EQ Option");
            Console.Write("Select Menu: ");
            
            switch (Command.Read())
            {
                case 0:
                    return c_HALT;
                case 1:
                    return _selectMusicCommand1.Switch();
                case 2:
                    return _selectMusicCommand2.Switch();
                case 3:
                    return _eqCommand.Switch();
                default:
                    return c_FAIL;
            }
        }
    }
}
