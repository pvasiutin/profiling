#pragma once

#include <stdint.h>

#include <string>

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
    size_t scope_index_{InvalidKey};
    bool already_closed_{false};
};

struct ScopeData
{
    uint64_t elapsed{0};
    std::string name;
};

size_t beginScope(const char* name);
void printScopes(uint64_t total_elapsed);

}

#define ConcatImpl(x, y) x##y
#define Concat(x, y) ConcatImpl(x, y)

#define TimeBlock(block_name) metrics::Scope Concat(scope_, __LINE__)(metrics::beginScope(block_name))

#define TimeFunction() TimeBlock(__func__)

#define TimeZoneBegin(zone_name) metrics::Scope Concat(scope_, zone_name)(metrics::beginScope(#zone_name))
#define TimeZoneEnd(zone_name) Concat(scope_, zone_name).close()
