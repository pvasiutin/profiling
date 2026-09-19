#include "PlatformMetrics.h"

namespace metrics
{

uint64_t getOsTimerFrequency()
{
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	return Freq.QuadPart;
}

uint64_t readOsTimer()
{
	LARGE_INTEGER Value;
	QueryPerformanceCounter(&Value);
	return Value.QuadPart;
}

uint64_t calculateCpuFrequency(uint64_t milliseconds_to_wait)
{
    uint64_t os_frequency = metrics::getOsTimerFrequency();

    uint64_t cpu_start = metrics::readCpuTimer();

    uint64_t os_start = metrics::readOsTimer();
    uint64_t os_end = 0;
    uint64_t os_elapsed = 0;
    uint64_t os_wait_time = os_frequency * milliseconds_to_wait / 1000;

    while (os_elapsed < os_wait_time)
    {
        os_end = metrics::readOsTimer();
        os_elapsed = os_end - os_start;
    }

    uint64_t cpu_end = metrics::readCpuTimer();
    uint64_t cpu_elapsed = cpu_end - cpu_start;
    uint64_t cpu_frequency = 0;
    if (os_elapsed != 0)
    {
        cpu_frequency = os_frequency * cpu_elapsed / os_elapsed;
    }

    return cpu_frequency;
}

double cpuTimerToMilliseconds(uint64_t cpu_elapsed, uint64_t cpu_frequency)
{
    return 1000.0 * double(cpu_elapsed) / double(cpu_frequency);
}

}
