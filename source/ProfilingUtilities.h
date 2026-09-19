#pragma once

#include <stdint.h>

#include <string>

#include "PlatformMetrics.h"

namespace metrics
{

class Scope
{
public:
    Scope(const uint32_t* map_key);
    ~Scope();

    void close();

private:
    const uint32_t* key_;;
    bool already_closed_{false};
};

struct ScopeData
{
    uint64_t elapsed{0};
    std::string name;
};

void beginScope(const uint32_t* scope_addr, const char* name);
void printScopes(uint64_t total_elapsed);

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
