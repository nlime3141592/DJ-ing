#pragma once

#include <chrono>

typedef std::chrono::steady_clock::time_point djChronoClock;
typedef std::chrono::microseconds::rep djChronoMCS;

#define CHRONO_NOW std::chrono::high_resolution_clock::now()
#define CHRONO_BEG_MCS CHRONO_NOW
#define CHRONO_LENGTH_MCS(beg, end) std::chrono::duration_cast<std::chrono::microseconds>(end - beg).count()