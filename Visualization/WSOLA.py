import numpy as np
import matplotlib.pyplot as plt
from scipy.signal.windows import hann

fs = 1000           # 샘플링 주파수
duration = 1.0      # 초
f0 = 5              # sine frequency
alpha = 1.1         # time-stretch factor (>1 느려짐)
frameSize = 200
hopIn = 100
hopOut = int(hopIn * alpha)
searchRange = 20

def CrossCorrelation(a, b, searchRange):
    maxCorr = -np.inf
    bestOffset = 0
    N = len(a)

    for offset in range(-searchRange, searchRange):
        if offset < 0:
            # a의 앞부분과 b의 뒷부분 맞춤
            a_slice = a[:N+offset]       # 길이 N+offset
            b_slice = b[-offset:]        # 길이 -offset
        elif offset > 0:
            a_slice = a[offset:]         # 길이 N-offset
            b_slice = b[:N-offset]       # 길이 N-offset
        else:
            a_slice = a
            b_slice = b
        
        # 길이가 동일한지 확인 후 dot
        if len(a_slice) != len(b_slice) or len(a_slice) == 0:
            continue
        
        corr = np.dot(a_slice, b_slice)
        
        if corr > maxCorr:
            maxCorr = corr
            bestOffset = offset
    
    return bestOffset

def wsola(x, frameSize, hopIn, hopOut, w, searchRange, alpha):
    N = frameSize
    yLen = int(len(x) * alpha) + N
    y = np.zeros(yLen)

    prevFrame = np.zeros(N)
    inPos = 0
    outPos = 0

    while inPos + N <= len(x):
        frame = x[inPos:inPos+N]
        offset = CrossCorrelation(prevFrame[hopOut:], frame, searchRange)
        y[outPos+offset:outPos+offset+N] += frame * w # overlap-add
        prevFrame = frame * w
        inPos += hopIn
        outPos += hopOut
    
    return y

t = np.arange(0, duration, 1/fs)
x = np.sin(2 * np.pi * f0 * t)

w = hann(frameSize, sym=False)

y = wsola(x, frameSize, hopIn, hopOut, w, searchRange, alpha)

plt.figure(figsize=(12,5))
plt.plot(t, x, label='Input')
t_out = np.arange(len(y)) / fs
plt.plot(t_out, y[:len(t_out)], label='WSOLA Output')
plt.xlabel('Time [s]')
plt.ylabel('Amplitude')
plt.legend()
plt.title(f'WSOLA Time-Stretch (alpha={alpha})')
plt.show()

y_norm = y / np.max(np.abs(y))