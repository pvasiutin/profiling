#include "ProfilingUtilities.h"

#include <stdio.h>

#include <vector>

namespace metrics
{

static std::vector<const uint32_t*> ScopeDataKeys;
static std::vector<ScopeData> ScopeDataValues;

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
    static size_t InvalidKey = size_t(-1);

    uint64_t scope_end = readCpuTimer();

    size_t existing_key = InvalidKey;
    for (size_t key_index = 0; key_index < ScopeDataKeys.size(); ++key_index)
    {
        if (ScopeDataKeys[key_index] == key_)
        {
            existing_key = key_index;
            break;
        }
    }

    if (existing_key != InvalidKey)
    {
        auto& scoped_data = ScopeDataValues[existing_key];
        scoped_data.elapsed = scope_end - scoped_data.elapsed;
    }

    already_closed_ = true;
}

void beginScope(const uint32_t* scope_addr, const char* name)
{
    uint64_t scope_start = readCpuTimer();

    ScopeData data;
    data.elapsed = scope_start;
    data.name = name;
    ScopeDataValues.emplace_back(std::move(data));

    ScopeDataKeys.push_back(scope_addr);
}

void printScopes(uint64_t total_elapsed)
{
    uint64_t total_scopes_elapsed{0};
    for (const auto& scope : ScopeDataValues)
    {
        printTimeElapsed(scope.name.c_str(), scope.elapsed, total_elapsed);
        total_scopes_elapsed += scope.elapsed;
    }
    printTimeElapsed("Scopes total", total_scopes_elapsed, total_elapsed);
}

}
