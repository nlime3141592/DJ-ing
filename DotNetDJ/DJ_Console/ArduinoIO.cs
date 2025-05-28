using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace nl
{
    public class ArduinoIO
    {
        private const int c_IDX_FLAG_EXIT = 0;

        public bool ExitFlag => _flags[c_IDX_FLAG_EXIT];
        public float W0 => _fWeights[0];

        private SerialPort _port;

        private bool[] _flags;
        private float[] _fWeights;

        public ArduinoIO(string portName)
        {
            _port = new SerialPort(portName);

            _flags = new bool[16];
            _fWeights = new float[12];
        }

        public static string InputGettingPortNameOrNull()
        {
            string[] portNames = SerialPort.GetPortNames();

            if (portNames == null || portNames.Length == 0)
            {
                return null;
            }

            Console.WriteLine("Serial Port List:");

            for (int i = 0; i < portNames.Length; ++i)
            {
                Console.WriteLine($"  {i}: {portNames[i]}");
            }

            Console.Write("Enter the Port Index: ");

            int index;

            if (!int.TryParse(Console.ReadLine(), out index) || index < 0 || index >= portNames.Length)
            {
                return null;
            }

            return portNames[index];
        }

        public void Open()
        {
            if (_port.IsOpen)
                return;

            _port.Open();
        }

        public void Update()
        {
            try
            {
                string line = _port.ReadLine();

                Parse(line);
            }
            catch (Exception)
            {
                _flags[c_IDX_FLAG_EXIT] = true;
            }
        }

        public void Close()
        {
            if (!_port.IsOpen)
                return;

            _port.Close();
        }

        private void Parse(string line)
        {
            int value;
            
            if (int.TryParse(line, out value))
            {
                _flags[c_IDX_FLAG_EXIT] |= (value & 1024) > 0;
                _fWeights[0] = (float)(value & 1023) / 1023.0f;
            }
        }
    }
}
