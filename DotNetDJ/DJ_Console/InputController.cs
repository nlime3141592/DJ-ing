using System;
using System.IO.Ports;

namespace nl
{
    public class InputController : Input
    {
        private const int c_STRUCT_SIZE = 16;

        #region Flag Definitions
        public const Byte c_FLAG_CFX_0 = 0x80;
        public const Byte c_FLAG_CFX_1 = 0x40;
        public const Byte c_FLAG_HALT = 0x01;
        #endregion

        #region Interfaces about Raw Data
        public Int32 DeltaSelect => _dSelect;
        public Byte EqHigh0 => _eq0;
        public Byte EqMid0 => _eq1;
        public Byte EqLow0 => _eq2;
        public Byte EqHigh1 => _eq3;
        public Byte EqMid1 => _eq4;
        public Byte EqLow1 => _eq5;
        public Byte ChannelFx0 => _fx0;
        public Byte ChannelFx1 => _fx1;
        public Byte VolumeFader0 => _vf0;
        public Byte VolumeFader1 => _vf1;
        public Byte CrossFader => _xf;
        public bool BtnCfx0 => (_btnFlag0 & c_FLAG_CFX_0) > 0;
        public bool BtnCfx1 => (_btnFlag0 & c_FLAG_CFX_1) > 0;
        public bool BtnHalt => (_btnFlag0 & c_FLAG_HALT) > 0;
        #endregion

        #region Raw Data
        private Int32 _dSelect;
        private Byte _eq0;
        private Byte _eq1;
        private Byte _eq2;
        private Byte _eq3;
        private Byte _eq4;
        private Byte _eq5;
        private Byte _fx0;
        private Byte _fx1;
        private Byte _vf0;
        private Byte _vf1;
        private Byte _xf;
        private Byte _btnFlag0;
        #endregion

        public InputController()
        : base(c_STRUCT_SIZE)
        {

        }

        public override int Update(SerialPort port)
        {
            int rdLength = base.Read(port);

            if (rdLength != c_STRUCT_SIZE)
                return -1;

            _dSelect = BitConverter.ToInt32(base._rdBuffer, 0);
            _eq0 = base._rdBuffer[4];
            _eq1 = base._rdBuffer[5];
            _eq2 = base._rdBuffer[6];
            _eq3 = base._rdBuffer[7];
            _eq4 = base._rdBuffer[8];
            _eq5 = base._rdBuffer[9];
            _fx0 = base._rdBuffer[10];
            _fx1 = base._rdBuffer[11];
            _vf0 = base._rdBuffer[12];
            _vf1 = base._rdBuffer[13];
            _xf = base._rdBuffer[14];
            _btnFlag0 = base._rdBuffer[15];

            return rdLength;
        }
    }
}
