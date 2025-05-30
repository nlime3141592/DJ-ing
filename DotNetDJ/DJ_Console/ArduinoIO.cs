using System;
using System.IO.Ports;

namespace nl
{
    public class ArduinoIO
    {
        private const int c_IDX_FLAG_EXIT = 0;

        public bool ExitFlag => _flags[c_IDX_FLAG_EXIT];
        public float W0 => _fWeights[0];
        public float W1 => _fWeights[1];
        public float W2 => _fWeights[2];
        public float W3 => _fWeights[3];
        public float W4 => _fWeights[4];
        public float W5 => _fWeights[5];
        public float W6 => _fWeights[6];
        public float W7 => _fWeights[7];

        private SerialPort _port;

        private byte[] _buffer;
        private bool[] _flags;
        private float[] _fWeights;

        public ArduinoIO(string portName)
        {
            _port = new SerialPort(portName);

            _buffer = new byte[1];
            _flags = new bool[16];
            _fWeights = new float[12];

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
                _port.Write(_buffer, 0, 1);
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
            string[] tokens = line.Split('/');
            uint value;

            for (int i = 0; i < 8; ++i)
            {
                if (uint.TryParse(tokens[i], out value))
                    _fWeights[i] = ((float)value / 1023.0f);
            }

            if (uint.TryParse(tokens[8], out value))
                _flags[c_IDX_FLAG_EXIT] |= value > 0;
        }
    }
}
