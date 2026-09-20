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
    Scope(const uint32_t* map_key);
    ~Scope();

    void close();

private:
    bool already_closed_{false};
    const uint32_t* key_;
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
    std::vector<const uint32_t*> scope_data_keys;
};

void beginProfile(uint64_t milliseconds_to_wait);
void endProfile();

void beginScope(const uint32_t* scope_addr, const char* name);

void printStats();

}

#define ConcatImpl(x, y) x##y
#define Concat(x, y) ConcatImpl(x, y)

#define TimeBlock(block_name) static uint32_t Concat(var_, __LINE__){0};    \
    metrics::beginScope(&Concat(var_, __LINE__), block_name);               \
    metrics::Scope Concat(scope_, __LINE__)(&Concat(var_, __LINE__))

#define TimeFunction() TimeBlock(__func__)

#define TimeZoneBegin(zone_name) static uint32_t Concat(var_, __LINE__){0};    \
    metrics::beginScope(&Concat(var_, __LINE__), #zone_name);                   \
    metrics::Scope Concat(scope_, zone_name)(&Concat(var_, __LINE__))

#define TimeZoneEnd(zone_name) Concat(scope_, zone_name).close()
