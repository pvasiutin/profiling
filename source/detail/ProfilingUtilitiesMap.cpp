#include "ProfilingUtilities.h"

#include <stdio.h>

#ifndef USE_UNORDERED
    #define USE_UNORDERED 1
#endif

#ifdef USE_UNORDERED
    #include <unordered_map>
#else
    #include <map>
#endif

namespace metrics
{

#ifdef USE_UNORDERED
    static std::unordered_map<const uint32_t*, ScopeData> ScopeDataMap;
#else
    static std::map<const uint32_t*, ScopeData> ScopeDataMap;
#endif

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

    auto& scoped_data = ScopeDataMap[key_];
    scoped_data.elapsed = scope_end - scoped_data.elapsed;
    already_closed_ = true;
}

void beginScope(const uint32_t* scope_addr, const char* name)
{
    uint64_t scope_begin = readCpuTimer();

    auto& data = ScopeDataMap[scope_addr];
    data.elapsed = scope_begin;
    data.name = name;
}

void printScopes(uint64_t total_elapsed)
{
    uint64_t total_scopes_elapsed{0};
    for (const auto& [_, scope] : ScopeDataMap)
    {
        printTimeElapsed(scope.name.c_str(), scope.elapsed, total_elapsed);
        total_scopes_elapsed += scope.elapsed;
    }
    printTimeElapsed("Scopes total", total_scopes_elapsed, total_elapsed);
}

}
