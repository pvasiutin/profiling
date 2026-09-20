#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "PlatformMetrics.h"

namespace metrics
{

class Scope
{
    static const size_t InvalidKey = size_t(-1);

public:
    Scope(size_t scope_index);
    ~Scope();

    void close();

private:
    bool already_closed_{false};
    size_t scope_index_{InvalidKey};
};

struct ScopeData
{
    uint64_t elapsed{0};
    std::string name;
};

struct Profiler
{
    uint64_t begin{0};
    uint64_t elapsed{0};

    uint64_t cpu_frequency{0};

    std::vector<ScopeData> scope_data_values;
};

void beginProfile(uint64_t milliseconds_to_wait);
void endProfile();

size_t beginScope(const char* name);

void printStats();

}

#define ConcatImpl(x, y) x##y
#define Concat(x, y) ConcatImpl(x, y)

#define TimeBlock(block_name) metrics::Scope Concat(scope_, __LINE__)(metrics::beginScope(block_name))

#define TimeFunction() TimeBlock(__func__)

#define TimeZoneBegin(zone_name) metrics::Scope Concat(scope_, zone_name)(metrics::beginScope(#zone_name))
#define TimeZoneEnd(zone_name) Concat(scope_, zone_name).close()
