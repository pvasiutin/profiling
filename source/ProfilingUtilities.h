#pragma once

#include <stdint.h>

namespace metrics
{

class Scope
{
public:
    static const size_t InvalidKey = size_t(-1);

    Scope(const uint32_t* map_key, const char* name);
    ~Scope();

    void close();

private:
    bool already_closed_{false};

    uint64_t begin_{0};
    uint64_t old_elapsed_inclusive_{0};

    const uint32_t* key_;
    const uint32_t* parent_key_;
};

void beginProfile(uint64_t milliseconds_to_wait);
void endProfile();

void printStats();

}

#define ConcatImpl(x, y) x##y
#define Concat(x, y) ConcatImpl(x, y)

#define TimeBlock(block_name) static uint32_t Concat(var_, __LINE__){0};          \
    metrics::Scope Concat(scope_, __LINE__)(&Concat(var_, __LINE__), block_name)

#define TimeFunction() TimeBlock(__func__)

#define TimeZoneBegin(zone_name) static uint32_t Concat(var_, __LINE__){0};       \
    metrics::Scope Concat(scope_, zone_name)(&Concat(var_, __LINE__), #zone_name)

#define TimeZoneEnd(zone_name) Concat(scope_, zone_name).close()
