#pragma once

#include <immintrin.h>
#include <stdint.h>
#include <Windows.h>

#include "audiodevice.h"
#include "audiochannel.h"
#include "LoopHID.h"

void AudioInit();
void AudioLoop();
void AudioFinal();