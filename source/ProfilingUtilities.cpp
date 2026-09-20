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

Scope::Scope(size_t scope_index) : scope_index_{scope_index}
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

    if (scope_index_ != InvalidKey)
    {
        auto& scoped_data = GlobalProfiler.scope_data_values[scope_index_];
        scoped_data.elapsed = scope_end - scoped_data.elapsed;
    }

    already_closed_ = true;
}

void beginProfile(uint64_t milliseconds_to_wait)
{
    GlobalProfiler.cpu_frequency = metrics::calculateCpuFrequency(milliseconds_to_wait);
    GlobalProfiler.scope_data_values.reserve(4096);
    GlobalProfiler.begin = readCpuTimer();
}

void endProfile()
{
    GlobalProfiler.elapsed = readCpuTimer() - GlobalProfiler.begin;
}

size_t beginScope(const char *name)
{
    uint64_t scope_start = readCpuTimer();

    ScopeData data;
    data.elapsed = scope_start;
    data.name = name;
    GlobalProfiler.scope_data_values.emplace_back(std::move(data));

    return GlobalProfiler.scope_data_values.size() - 1;
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
