#pragma once

#include <chrono>

#define CHRONO_NOW_MCS std::chrono::high_resolution_clock::now()
#define CHRONO_BEG_MCS CHRONO_NOW_MCS
#define CHRONO_END_MCS(beg) std::chrono::duration_cast<std::chrono::microseconds>(CHRONO_NOW_MCS - beg).count();