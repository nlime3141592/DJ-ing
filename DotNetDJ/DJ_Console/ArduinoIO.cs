using System;
using System.IO.Ports;

namespace nl
{
    public class ArduinoIO
    {
        public const uint c_FLAG_EXIT = 0x00000001;

        public bool ExitFlag => (_flag & c_FLAG_EXIT) > 0;

        public float W0 => _fWeights[0];
        public float W1 => _fWeights[1];
        public float W2 => _fWeights[2];
        public float W3 => _fWeights[3];
        public float W4 => _fWeights[4];
        public float W5 => _fWeights[5];
        public float W6 => _fWeights[6];
        public float W7 => _fWeights[7];

        private SerialPort _port;

        private uint _flag;
        private byte[] _buffer;

        private float[] _fWeights;

        public ArduinoIO(string portName)
        {
            _port = new SerialPort(portName);

            _flag = 0;
            _buffer = new byte[8];

            _fWeights = new float[8];

            _buffer[0] = 0;
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
                int rd = 0;
                int n = 1;

                _port.Write(_buffer, 0, 1);

                rd = 0;
                n = 1;
                while ((rd += _port.Read(_buffer, rd, n - rd)) < n) ;
                _flag = (uint)_buffer[0];

                rd = 0;
                n = 8;
                while ((rd += _port.Read(_buffer, rd, n - rd)) < n) ;
                for (int i = 0; i < n; ++i)
                {
                    _fWeights[i] = (float)_buffer[i] / 255.0f;
                }
            }
            catch (Exception)
            {
                _flag |= c_FLAG_EXIT;
            }
        }

        public void Close()
        {
            if (!_port.IsOpen)
                return;

            _port.Close();
        }
    }
}
