import numpy as np
import matplotlib.pyplot as plt

from scipy.signal import lfilter

domainLength = 40
duration = 10
samplePerSec = 10

windowLength = 5

def inputSignal0(t):
    y = np.sin(t)
    y[duration * samplePerSec:] = 0
    return y

def inputSignal1(t):
    y = np.sin(t)
    dur = duration * samplePerSec
    beg = dur
    end = 2 * dur

    y[0:dur-1] = 0
    y[end:] = 0
    return y

def hannWindow(t):
    y = 0.5 - 0.5 * np.cos(2 * np.pi * t / windowLength)
    return y

# sigma(f[x] * g[i - x])
def conv(f, g, fx0, fx1, gx0, gx1, bufferLength):
    df = fx1 - fx0
    dg = gx1 - gx0

    convCount = df + dg - 1

    y = np.empty(bufferLength, dtype=float)

    for i in range(df + dg - 1):
        for j in range(dg):
            k = i - j
            if k < 0:
                break
            y[fx0 + i] += f[fx0 + k] * g[gx0 + j]

    return y

t0 = np.linspace(0, domainLength, domainLength * samplePerSec)
t1 = np.linspace(0, duration, duration * samplePerSec)
t2 = np.linspace(0, windowLength, windowLength * samplePerSec)

yw = hannWindow(t2)
y0 = inputSignal0(t0)
y1 = conv(y0, yw, 0, duration * samplePerSec, 0, windowLength * samplePerSec, len(t0))
y2 = inputSignal1(t0)
y3 = conv(y2, yw, duration * samplePerSec, 2 * duration * samplePerSec, 0, windowLength * samplePerSec, len(t0))

### Plotting
fig, ax = plt.subplots(2, 3, figsize=(16, 8))

# inputs
ax[0, 0].plot(t0, y0)
ax[0, 1].plot(t0, y1)
ax[0, 2].plot(t2, yw)

ax[0, 0].set_title("input 0")
ax[0, 1].set_title("output 0")
ax[0, 2].set_title("window, size == 5")

# outputs
ax[1, 0].plot(t0, y2)
ax[1, 1].plot(t0, y3)
ax[1, 2].plot(t0,  y1 + y3)

ax[1, 0].set_title("input 1")
ax[1, 1].set_title("output 1")
ax[1, 2].set_title("sum of outputs")

plt.show()