#include "ProfilingUtilities.h"

#include <stdio.h>

//#include <unordered_map>
#include <map>

namespace metrics
{

static std::map<const uint32_t*, ScopeData> ScopeDataMap;

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
    auto& scoped_data = ScopeDataMap[key_];
    scoped_data.elapsed = readCpuTimer() - scoped_data.elapsed;
    already_closed_ = true;
}

void beginScope(const uint32_t* scope_addr, const char* name)
{
    auto& data = ScopeDataMap[scope_addr];
    data.elapsed =  readCpuTimer();
    data.name = name;
}

void printScopes(uint64_t total_elapsed)
{
    for (const auto& [_, scope] : ScopeDataMap)
    {
        printTimeElapsed(scope.name.c_str(), scope.elapsed, total_elapsed);
    }
}

}
