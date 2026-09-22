#include "ProfilingUtilities.h"

#include <stdio.h>

#include <vector>

namespace metrics
{

static Profiler GlobalProfiler;
static const uint32_t* GlobalParent{0};

namespace
{

void printTimeElapsed(const char * label, uint64_t elapsed, uint64_t total_elapsed)
{
    double percent = 100.0 * (double(elapsed) / double(total_elapsed));
    printf("%s: %llu (%0.2f%%)\n", label, elapsed, percent);
}

void printTimeElapsed(const ScopeData& data, uint64_t total_elapsed)
{
    uint64_t raw_elapsed = data.elapsed - data.elapsed_children;
    double percent = 100.0 * (double(raw_elapsed) / double(total_elapsed));
    printf("%s[%llu]: %llu (%0.2f%%", data.name.c_str(), data.hit_count, raw_elapsed, percent);
    if (data.elapsed_children)
    {
        double percent_with_children = 100.0 * (double(data.elapsed) / double(total_elapsed));
        printf(", %.2f%% w/children", percent_with_children);
    }
    printf(")\n");
}

size_t findExistingKeyIndex(const uint32_t* key)
{
    const auto& keys = GlobalProfiler.scope_data_keys;
    for (size_t key_index = 0; key_index < keys.size(); ++key_index)
    {
        if (keys[key_index] == key)
        {
            return key_index;
        }
    }
    return Scope::InvalidKey;
}

}

Scope::Scope(const uint32_t *map_key) : key_{map_key}
{
    parent_key_ = GlobalParent;
    GlobalParent = map_key;

    begin = readCpuTimer();
}

Scope::~Scope()
{
    if (!already_closed_)
    {
        close();
    }
}

void Scope::close()
{
    uint64_t elapsed = readCpuTimer() - begin;

    GlobalParent = parent_key_;
    const auto existing_parent_key = findExistingKeyIndex(parent_key_);
    if (existing_parent_key != InvalidKey)
    {
        auto& parent_scoped_data = GlobalProfiler.scope_data_values[existing_parent_key];
        parent_scoped_data.elapsed_children += elapsed;
    }

    const auto existing_key = findExistingKeyIndex(key_);
    if (existing_key != InvalidKey)
    {
        auto& scoped_data = GlobalProfiler.scope_data_values[existing_key];
        scoped_data.elapsed += elapsed;
        scoped_data.hit_count += 1;
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
    const auto existing_key = findExistingKeyIndex(scope_addr);
    if ( existing_key == Scope::InvalidKey)
    {
        auto& keys = GlobalProfiler.scope_data_keys;
        keys.push_back(scope_addr);

        auto& data = GlobalProfiler.scope_data_values.emplace_back();
        data.name = name;
    }
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
        printTimeElapsed(scope, total_elapsed);
        total_scopes_elapsed += (scope.elapsed - scope.elapsed_children);
    }
    printTimeElapsed("Scopes total", total_scopes_elapsed, total_elapsed);
}

}
