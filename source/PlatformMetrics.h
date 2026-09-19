#pragma once

#include <stdint.h>
#include <intrin.h>
#include <windows.h>

namespace metrics
{

uint64_t getOsTimerFrequency();
uint64_t readOsTimer();

inline uint64_t readCpuTimer()
{
    return __rdtsc();
}

uint64_t calculateCpuFrequency(uint64_t milliseconds_to_wait);

double cpuTimerToMilliseconds(uint64_t cpu_elapsed, uint64_t cpu_frequency);

}
