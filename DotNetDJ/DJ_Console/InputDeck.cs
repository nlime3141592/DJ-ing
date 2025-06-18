using System;
using System.IO.Ports;

namespace nl
{
    public class InputDeck : Input
    {
        private const int c_STRUCT_SIZE = 6;

        #region Flag Definitions
        public const Byte c_FLAG_SHIFT = 0x80;
        public const Byte c_FLAG_PLAY = 0x40;
        public const Byte c_FLAG_CUE = 0x20;
        public const Byte c_FLAG_PAD_0 = 0x10;
        public const Byte c_FLAG_PAD_1 = 0x08;
        public const Byte c_FLAG_PAD_2 = 0x04;
        public const Byte c_FLAG_PAD_3 = 0x02;
        #endregion

        #region Interfaces about Raw Data
        public Int32 DeltaWheel => _dWheel;
        public Byte Tempo => _tempo;
        public bool BtnShift => (_btnFlag0 & c_FLAG_SHIFT) > 0;
        public bool BtnPlay => (_btnFlag0 & c_FLAG_PLAY) > 0;
        public bool BtnCue => (_btnFlag0 & c_FLAG_CUE) > 0;
        public bool BtnPad0 => (_btnFlag0 & c_FLAG_PAD_0) > 0;
        public bool BtnPad1 => (_btnFlag0 & c_FLAG_PAD_1) > 0;
        public bool BtnPad2 => (_btnFlag0 & c_FLAG_PAD_2) > 0;
        public bool BtnPad3 => (_btnFlag0 & c_FLAG_PAD_3) > 0;
        #endregion

        #region Raw Data
        private Int32 _dWheel;
        private Byte _tempo;
        private Byte _btnFlag0;
        #endregion

        public InputDeck()
        : base(c_STRUCT_SIZE)
        {
            
        }

        public override int Update(SerialPort port)
        {
            int rdLength = base.Read(port);

            if (rdLength != c_STRUCT_SIZE)
                return -1;

            // 아두이노에서 보낸 덱 구조체를 읽음
            _dWheel = BitConverter.ToInt32(base._rdBuffer, 0);
            _tempo = base._rdBuffer[4];
            _btnFlag0 = base._rdBuffer[5];

            return rdLength;
        }
    }
}
