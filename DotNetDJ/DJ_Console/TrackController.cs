using NAudio;
using NAudio.Utils;
using NAudio.Wave;
using nl.AudioFilter;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace nl
{
    public class TrackController
    {
        public float[] source;
        public float[] buffer;

        public IAudioFilter padFX;

        public float fVolume;

        public IAudioFilter eqL;
        public IAudioFilter eqM;
        public IAudioFilter eqH;

        public IAudioFilter cFX;

        public float xVolume;

        public int coefSize;
        public float[] coefA;
        public float[] coefB;
        public float[] feedbackX;
        public float[] feedbackY;

        public float _sPointer;
        public float _bPointer;








        public WaveOutBuffer[] Buffers => _buffers;
        public PlaybackState PlaybackState => _playbackState;
        public int NumberOfBuffers { get; set; } = 2;
        public int DesiredLatency { get; set; } = 300;

        public event EventHandler<StoppedEventArgs> PlaybackStopped;

        private readonly SynchronizationContext _syncContext;

        private object _waveOutLock;

        private IntPtr _hWaveOut;
        private IWaveProvider _source;

        private WaveOutBuffer[] _buffers;
        private PlaybackState _playbackState;
        private AutoResetEvent _callbackEvent;

        public TrackController()
        {
            _waveOutLock = new object();
        }

        ~TrackController()
        {
            Dispose(false);
            System.Diagnostics.Debug.Assert(false, "TrackController device was not closed");
        }

        public void Init(IntPtr hWaveOut, IWaveProvider source)
        {
            if (_playbackState != PlaybackState.Stopped)
            {
                Stop();
            }
            if (_hWaveOut != IntPtr.Zero)
            {
                DisposeBuffers();
                CloseWaveOut();
            }

            _hWaveOut = hWaveOut;
            _source = source;

            _buffers = new WaveOutBuffer[NumberOfBuffers];
            _playbackState = PlaybackState.Stopped;
            _callbackEvent = new AutoResetEvent(false);

            int bufferSize = source.WaveFormat.ConvertLatencyToByteSize((DesiredLatency + NumberOfBuffers - 1) / NumberOfBuffers);

            for (int i = 0; i < NumberOfBuffers; ++i)
            {
                _buffers[i] = new WaveOutBuffer(hWaveOut, bufferSize, source, _waveOutLock);
            }
        }

        public void Dispose()
        {
            GC.SuppressFinalize(this);
            Dispose(true);
        }

        public float Process()
        {
            float x = 0;
            float y = 0;

            for (int i = 0; i < coefSize; ++i)
            {
                x += coefB[i] * feedbackX[i];
                y -= coefA[i] * feedbackY[i];
            }

            return x + y;
        }

        private void Apply()
        {
            // Implement DSP Chain
            for (int i = 0; i < buffer.Length; ++i)
            {
                // 1. Performance Pad FX
                buffer[i] = padFX.Process(buffer[i]);

                // 2. Volume Fader
                buffer[i] *= fVolume;

                // 3. Channel EQ
                buffer[i] = eqL.Process(buffer[i]);
                buffer[i] = eqM.Process(buffer[i]);
                buffer[i] = eqH.Process(buffer[i]);

                // 4. Channel FX
                buffer[i] = cFX.Process(buffer[i]);

                // 5. Crossfader
                buffer[i] *= xVolume;
            }
        }

        public void TogglePlaybackState()
        {
            if (_buffers == null || _source == null)
            {
                throw new InvalidOperationException("Must call Init first");
            }

            switch (_playbackState)
            {
                case PlaybackState.Stopped:
                    _playbackState = PlaybackState.Playing;
                    _callbackEvent.Set();
                    ThreadPool.QueueUserWorkItem(state => TrackThread(), null);
                    break;
                case PlaybackState.Playing:
                    Pause();
                    break;
                case PlaybackState.Paused:
                    Play();
                    break;
                default:
                    System.Diagnostics.Debug.Assert(false, "TogglePlaybackState");
                    break;
            }
        }

        protected void Dispose(bool disposing)
        {
            if (_playbackState != PlaybackState.Stopped)
            {
                Stop();
            }

            if (disposing)
            {
                DisposeBuffers();
            }

            CloseWaveOut();
        }

        private void TrackThread()
        {
            Exception exception = null;

            try
            {
                TrackPlayback();
            }
            catch (Exception ex)
            {
                exception = ex;
            }
            finally
            {
                _playbackState = PlaybackState.Stopped;
                RaisePlaybackStoppedEvent(exception);
            }
        }

        private void TrackPlayback()
        {
            while (_playbackState != PlaybackState.Stopped)
            {
                if (!_callbackEvent.WaitOne(DesiredLatency))
                {
                    if (_playbackState == PlaybackState.Playing)
                    {
                        System.Diagnostics.Debug.WriteLine("WARNING: TrackController callback event timeout");
                    }
                }

                if (_playbackState == PlaybackState.Playing)
                {
                    bool queued = false;

                    for (int i = 0; i < _buffers.Length; ++i)
                    {
                        queued |= _buffers[i].InQueue || _buffers[i].OnDone();
                    }

                    // The source get to the end.
                    if (!queued)
                    {
                        _playbackState = PlaybackState.Stopped;
                        _callbackEvent.Set();
                    }
                }
            }
        }

        private void RaisePlaybackStoppedEvent(Exception ex)
        {
            EventHandler<StoppedEventArgs> handler = null;

            if (handler != null)
            {
                if (_syncContext == null)
                {
                    handler(this, new StoppedEventArgs(ex));
                }
                else
                {
                    _syncContext.Post(state => handler(this, new StoppedEventArgs(ex)), null);
                }
            }
        }

        private void Play()
        {
            MmResult result;

            lock (_waveOutLock)
            {
                result = WaveInterop.waveOutRestart(_hWaveOut);
            }

            if (result != MmResult.NoError)
            {
                throw new MmException(result, "waveOutRestart");
            }

            _playbackState = PlaybackState.Playing;
            _callbackEvent.Set();
        }

        private void Pause()
        {
            MmResult result;

            _playbackState = PlaybackState.Paused;

            lock (_waveOutLock)
            {
                result = WaveInterop.waveOutPause(_hWaveOut);
            }

            if (result != MmResult.NoError)
            {
                throw new MmException(result, "waveOutPause");
            }
        }

        private void Stop()
        {
            _playbackState = PlaybackState.Stopped;
            MmResult result;

            lock (_waveOutLock)
            {
                result = WaveInterop.waveOutReset(_hWaveOut);
            }

            if (result != MmResult.NoError)
            {
                throw new MmException(result, "waveOutReset");
            }

            _callbackEvent.Set();
        }

        private void CloseWaveOut()
        {
            if (_callbackEvent != null)
            {
                _callbackEvent.Close();
                _callbackEvent = null;
            }

            lock (_waveOutLock)
            {
                if (_hWaveOut != IntPtr.Zero)
                {
                    WaveInterop.waveOutClose(_hWaveOut);
                    _hWaveOut = IntPtr.Zero;
                }
            }
        }

        private void DisposeBuffers()
        {
            if (_buffers != null)
            {
                for (int i = 0; i < _buffers.Length; ++i)
                {
                    _buffers[i].Dispose();
                }

                _buffers = null;
            }
        }
    }
}
