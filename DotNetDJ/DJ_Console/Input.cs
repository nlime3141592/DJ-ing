using System;
using System.IO.Ports;

namespace nl
{
    public abstract class Input
    {
        protected Byte[] _rdBuffer;

        protected Input(int bufferSize)
        {
            _rdBuffer = new Byte[bufferSize];
        }

        public abstract int Update(SerialPort port);

        protected int Read(SerialPort port)
        {
            int rdLengthTotal = 0;
            int rdLength = -1;

            while (rdLengthTotal < _rdBuffer.Length)
            {
                if (!port.IsOpen)
                    return -1;

                try
                {
                    rdLength = port.Read(_rdBuffer, rdLengthTotal, _rdBuffer.Length - rdLengthTotal);
                }
                catch (InvalidOperationException)
                {
                    // 포트가 열려 있지 않음
                    return -1;
                }
                catch (TimeoutException)
                {
                    // 데이터를 읽을 수 없음, 타임 아웃
                    return -1;
                }
                catch (Exception)
                {
                    // 알 수 없는 예외 발생
                    return -1;
                }

                if (rdLength < 0)
                    return -1;

                rdLengthTotal += rdLength;
            }

            return rdLengthTotal;
        }
    }
}
