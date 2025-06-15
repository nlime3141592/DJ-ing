using System;
using System.ComponentModel;
using System.IO;
using System.IO.Ports;

namespace nl
{
    public class ArduinoIO
    {
        private const int c_BAUD_RATE = 115200;

        public InputController Controller => _inputs[0] as InputController;
        public InputDeck Deck1 => _inputs[1] as InputDeck;
        public InputDeck Deck2 => _inputs[2] as InputDeck;

        private SerialPort _port;
        private Input[] _inputs;
        private byte[] _wrBuffer;

        public ArduinoIO()
        {
            _inputs = new Input[3];
            _inputs[0] = new InputController();
            _inputs[1] = new InputDeck();
            _inputs[2] = new InputDeck();
            _wrBuffer = new byte[1];
        }

        public static string[] GetPortNames()
        {
            try
            {
                string[] portNames = SerialPort.GetPortNames();

                if (portNames == null)
                    return Array.Empty<string>();

                return portNames;
            }
            catch (Win32Exception)
            {
                // 포트를 찾을 수 없음
                return Array.Empty<string>();
            }
            catch (Exception)
            {
                // 알 수 없는 오류
                return Array.Empty<string>();
            }
        }

        public void Open(string portName)
        {
            if (_port != null && _port.IsOpen)
                _port.Close();

            try
            {
                _port = new SerialPort(portName, c_BAUD_RATE);
                _port.Open();
            }
            catch (UnauthorizedAccessException)
            {
                // 1. 포트 액세스 거부
                // 2. 다른 코드에서 이미 사용 중인 포트
                _port = null;
            }
            catch (ArgumentOutOfRangeException)
            {
                // 통신을 위한 속성 값이 잘못 지정됨
                _port = null;
            }
            catch (ArgumentException)
            {
                // 1. 포트 이름이 잘못됨
                // 2. 포트의 파일 형식이 지원되지 않음
                _port = null;
            }
            catch (IOException)
            {
                // 포트를 열 수 없음
                _port = null;
            }
            catch (InvalidOperationException)
            {
                // 현재 프로세스에서 사용 중인 포트로서, 이미 열려 있음
                _port = null;
            }
            catch (Exception)
            {
                // 알 수 없는 예외
                _port = null;
            }

            if (_port != null && !_port.IsOpen)
            {
                _port.Dispose();
                _port = null;
            }
        }

        public void Close()
        {
            if (_port == null)
                return;

            if (_port.IsOpen)
                _port.Close();

            _port.Dispose();
            _port = null;
        }

        public int Update()
        {
            int rdLengthTotal = 0;
            int rdLength = -1;

            _port.Write(_wrBuffer, 0, 1);

            for (int i = 0; i < _inputs.Length; ++i)
            {
                rdLength = _inputs[i].Update(_port);

                if (rdLength < 0)
                    return -1;

                rdLengthTotal += rdLength;
            }

            return rdLengthTotal;
        }
    }
}
