using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace nl.ConsoleCommand
{
    internal class SelectEQ : Command
    {
        public override int Switch()
        {
            Channel c = null;

            Console.WriteLine("  1. Apply EQ on Channel #1");
            Console.WriteLine("  2. Apply EQ on Channel #2");
            Console.Write("Select Menu: ");

            switch (Command.Read())
            {
                case 1:
                    c = Program.controller.c1;
                    break;
                case 2:
                    c = Program.controller.c2;
                    break;
                default:
                    return Command.c_FAIL;
            }

            Console.WriteLine("  1. Peaking/-20dB/1000Hz, LowShelving/-20dB/2000Hz, HighShelving/-20dB/2000Hz");
            Console.WriteLine("  2. Peaking/0dB/1000Hz, LowShelving/0dB/200Hz, HighShelving/0dB/20000Hz");
            Console.WriteLine("  3. Peaking/+3dB/1000Hz, LowShelving/+3dB/200Hz, HighShelving/+3dB/20000Hz");
            Console.Write("Select Menu: ");

            switch (Command.Read())
            {
                case 1:
                    c.pf.Gain = -20.0f;
                    c.pf.CutoffHz = 1000.0f;
                    c.lsf.Gain = -20.0f;
                    c.lsf.CutoffHz = 2000.0f;
                    c.hsf.Gain = -20.0f;
                    c.hsf.CutoffHz = 2000.0f;
                    break;
                case 2:
                    c.pf.Gain = 0.0f;
                    c.pf.CutoffHz = 1000.0f;
                    c.lsf.Gain = 0.0f;
                    c.lsf.CutoffHz = 200.0f;
                    c.hsf.Gain = 0.0f;
                    c.hsf.CutoffHz = 20000.0f;
                    break;
                case 3:
                    c.pf.Gain = 3.0f;
                    c.pf.CutoffHz = 1000.0f;
                    c.lsf.Gain = 3.0f;
                    c.lsf.CutoffHz = 200.0f;
                    c.hsf.Gain = 3.0f;
                    c.hsf.CutoffHz = 20000.0f;
                    break;
                default:
                    return Command.c_FAIL;
            }

            return Command.c_SUCCESS;
        }
    }
}
