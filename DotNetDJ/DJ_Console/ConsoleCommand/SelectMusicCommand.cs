using NAudio.Wave;
using System;
using System.IO;
using System.Linq;

namespace nl.ConsoleCommand
{
    public class SelectMusicCommand : Command
    {
        private const string c_ROOT_DIRECTORY = @"C:\Test\";

        private int _channelNumber;

        public SelectMusicCommand(int channelNumber)
        {
            _channelNumber = channelNumber;
        }

        public override int Switch()
        {
            string file = SelectAudioFile(c_ROOT_DIRECTORY);

            if (file == string.Empty)
                return Command.c_FAIL;

            AudioFileReader source = new AudioFileReader(file);

            switch (_channelNumber)
            {
                case 1:
                    Program.controller.isPlay1 = false;
                    Program.controller.SetSource(source, 1);
                    return Command.c_SUCCESS;
                case 2:
                    Program.controller.isPlay2 = false;
                    Program.controller.SetSource(source, 2);
                    return Command.c_SUCCESS;
                default:
                    return Command.c_FAIL;
            }
        }

        private string SelectAudioFile(string rootDirectory)
        {
            string[] wav = Directory.GetFiles(rootDirectory, "*.wav");
            string[] mp3 = Directory.GetFiles(rootDirectory, "*.mp3");

            string[] files = wav.Concat(mp3).ToArray();

            for (int i = 0; i < files.Length; ++i)
            {
                Console.WriteLine($"{i + 1:D02}: {files[i]}");
            }

            Console.Write("Select Music: ");

            int index;

            if (!int.TryParse(Console.ReadLine(), out index))
            {
                return string.Empty;
            }

            return files[index - 1];
        }
    }
}
