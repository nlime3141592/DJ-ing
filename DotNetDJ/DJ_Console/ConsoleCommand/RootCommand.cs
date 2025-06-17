using System;

namespace nl.ConsoleCommand
{
    public class RootCommand : Command
    {
        private SelectMusicCommand _selectMusicCommand1;
        private SelectMusicCommand _selectMusicCommand2;

        public RootCommand()
        {
            _selectMusicCommand1 = new SelectMusicCommand(1);
            _selectMusicCommand2 = new SelectMusicCommand(2);
        }

        public override int Switch()
        {
            Console.Write("Input Command: ");
            
            switch (Command.Read())
            {
                case 0:
                    return c_HALT;
                case 1:
                    return _selectMusicCommand1.Switch();
                case 2:
                    return _selectMusicCommand2.Switch();
                default:
                    return c_FAIL;
            }
        }
    }
}
