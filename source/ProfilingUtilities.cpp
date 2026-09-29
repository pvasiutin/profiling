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
    double percent = 100.0 * (double(data.elapsed_exclusive) / double(total_elapsed));
    printf("  %s[%llu]: %llu (%0.2f%%", data.name.c_str(), data.hit_count, data.elapsed_exclusive, percent);
    if (data.elapsed_inclusive != data.elapsed_exclusive)
    {
        double percent_with_children = 100.0 * (double(data.elapsed_inclusive) / double(total_elapsed));
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

Scope::Scope(const uint32_t *map_key, const char* name) : key_{map_key}
{
    parent_key_ = GlobalParent;
    GlobalParent = map_key;

    if (const auto existing_key = findExistingKeyIndex(key_); existing_key == Scope::InvalidKey)
    {
        auto& keys = GlobalProfiler.scope_data_keys;
        keys.push_back(key_);

        auto& data = GlobalProfiler.scope_data_values.emplace_back();
        data.name = name;
    }
    else
    {
        auto& data = GlobalProfiler.scope_data_values[existing_key];
        old_elapsed_inclusive_ = data.elapsed_inclusive;
    }

    begin_ = readCpuTimer();
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
    uint64_t elapsed = readCpuTimer() - begin_;

    GlobalParent = parent_key_;

    const auto& keys = GlobalProfiler.scope_data_keys;
    for (size_t key_index = 0; key_index < keys.size(); ++key_index)
    {
        const auto key = keys[key_index];
        if (key == parent_key_)
        {
            auto& parent_scoped_data = GlobalProfiler.scope_data_values[key_index];
            parent_scoped_data.elapsed_exclusive -= elapsed;
        }
        if (key == key_)
        {
            auto& key_scoped_data = GlobalProfiler.scope_data_values[key_index];
            key_scoped_data.elapsed_exclusive += elapsed;
            key_scoped_data.elapsed_inclusive = old_elapsed_inclusive_ + elapsed;
            key_scoped_data.hit_count += 1;
        }
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
        total_scopes_elapsed += scope.elapsed_exclusive;
    }
    printTimeElapsed("Scopes total", total_scopes_elapsed, total_elapsed);
}

}
