#include "ProfilingUtilities.h"

#include <stdio.h>

#include <vector>

namespace metrics
{

static Profiler GlobalProfiler;

namespace
{

void printTimeElapsed(const char * label, uint64_t elapsed, uint64_t total_elapsed)
{
    double percent = 100.0 * (double(elapsed) / double(total_elapsed));
    printf("%s: %llu (%0.2f%%)\n", label, elapsed, percent);
}

}

Scope::Scope(const uint32_t *map_key) : key_{map_key}
{}

Scope::~Scope()
{
    if (!already_closed_)
    {
        close();
    }
}

void Scope::close()
{
    uint64_t scope_end = readCpuTimer();

    size_t existing_key = InvalidKey;
    const auto& keys = GlobalProfiler.scope_data_keys;
    for (size_t key_index = 0; key_index < keys.size(); ++key_index)
    {
        if (keys[key_index] == key_)
        {
            existing_key = key_index;
            break;
        }
    }

    auto& values = GlobalProfiler.scope_data_values;
    if (existing_key != InvalidKey)
    {
        auto& scoped_data = values[existing_key];
        scoped_data.elapsed = scope_end - scoped_data.elapsed;
    }

    already_closed_ = true;
}

void beginProfile(uint64_t milliseconds_to_wait)
{
    GlobalProfiler.cpu_frequency = metrics::calculateCpuFrequency(milliseconds_to_wait);
    GlobalProfiler.scope_data_values.reserve(1024);
    GlobalProfiler.scope_data_keys.reserve(1024);
    GlobalProfiler.begin = readCpuTimer();
}

void endProfile()
{
    GlobalProfiler.elapsed = readCpuTimer() - GlobalProfiler.begin;
}

void beginScope(const uint32_t* scope_addr, const char* name)
{
    auto& keys = GlobalProfiler.scope_data_keys;
    keys.push_back(scope_addr);

    auto& values = GlobalProfiler.scope_data_values;
    auto& data = values.emplace_back();
    data.name = name;
    data.elapsed = readCpuTimer();;
}

void printStats()
{
    uint64_t total_elapsed = GlobalProfiler.elapsed;
    uint64_t cpu_frequency = GlobalProfiler.cpu_frequency;

    if (cpu_frequency)
    {
        printf("\nTotal time: %0.4fms (CPU: %.2fGHz)\n", metrics::cpuTimerToMilliseconds(total_elapsed, cpu_frequency), double(cpu_frequency) / 1000000000);
    }

    uint64_t total_scopes_elapsed{0};
    for (const auto& scope : GlobalProfiler.scope_data_values)
    {
        printTimeElapsed(scope.name.c_str(), scope.elapsed, total_elapsed);
        total_scopes_elapsed += scope.elapsed;
    }
    printTimeElapsed("Scopes total", total_scopes_elapsed, total_elapsed);
}

}
