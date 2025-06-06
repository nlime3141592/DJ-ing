using NAudio;
using NAudio.Wave;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace nl
{
    public class MasterController
    {
        public const int c_TRACK_COUNT = 2;

        public int DesiredLatency { get; set; } = 300;
        public int NumberOfBuffers { get; set; } = 2;

        public int MasterDeviceNumber { get; set; } = -1;
        public int MonitorDeviceNumber { get; set; } = -1;

        public TrackController[] tracks;

        private object _waveOutLock;

        public MasterController()
        {
            tracks = new TrackController[c_TRACK_COUNT];

            for (int i = 0; i < c_TRACK_COUNT; ++i)
            {
                tracks[i] = new TrackController();
            }
        }

        public void Init(IWaveProvider source, int trackIndex)
        {
            TrackController track = tracks[trackIndex];

            tracks[trackIndex].Init(source);
        }

        private void SetMasterDevice(int deviceNumber)
        {
            MmResult result;

            lock (_waveOutLock)
            {
                result = WaveInterop.waveOutOpenWindow(out _)
            }
        }
    }
}
