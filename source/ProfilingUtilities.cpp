#include "ProfilingUtilities.h"

#include <stdio.h>

#include <vector>

namespace metrics
{

static std::vector<ScopeData> ScopeDataValues;

namespace
{

void printTimeElapsed(const char * label, uint64_t elapsed, uint64_t total_elapsed)
{
    double percent = 100.0 * (double(elapsed) / double(total_elapsed));
    printf("%s: %llu (%0.2f%%)\n", label, elapsed, percent);
}

}

Scope::Scope(size_t scope_index) : scope_index_{scope_index}
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

    if (scope_index_ != InvalidKey)
    {
        auto& scoped_data = ScopeDataValues[scope_index_];
        scoped_data.elapsed = scope_end - scoped_data.elapsed;
    }

    already_closed_ = true;
}

size_t beginScope(const char* name)
{
    uint64_t scope_start = readCpuTimer();

    ScopeData data;
    data.elapsed = scope_start;
    data.name = name;
    ScopeDataValues.emplace_back(std::move(data));

    return ScopeDataValues.size() - 1;
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
