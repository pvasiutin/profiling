#include <stdio.h>
#include <memory>
#include <fstream>
#include <filesystem>

#include "PlatformMetrics.h"
#include "ProfilingUtilities.h"

struct Foo
{
    size_t bar{};
    size_t buzz{};

    void printVals()
    {
        printf("bar: %llu\n", bar);
        printf("buzz: %llu\n", buzz);
    }
};

void printTimeElapsed(const char * label, uint64_t scope_bein, uint64_t scope_end, uint64_t total_elapsed)
{
    uint64_t elapsed = scope_end - scope_bein;
    double percent = 100.0 * (double(elapsed) / double(total_elapsed));
    printf("%s: %llu (%0.2f%%)\n", label, elapsed, percent);
}

void unique_ptr_call()
{
    TimeFunction();
    for (uint64_t i = 0; i < 1000; ++i)
    {
        std::unique_ptr unique_foo = std::make_unique<Foo>();
    }
}

void printf_call()
{
    TimeFunction();
    printf("Resulted counter: %llu\n", uint64_t(69));
}

void write_file()
{
    TimeFunction();
    std::ofstream ofs("test_file.txt");
    ofs << "kek";
    ofs.close();
}

void remove_file()
{
    TimeFunction();
    std::filesystem::remove("test_file.txt");
}

int main(int arg_count, char** args)
{
    uint64_t milliseconds_to_wait = 100;
    if (arg_count == 2)
    {
        milliseconds_to_wait = atol(args[1]);
    }

    auto cpu_frequency = metrics::calculateCpuFrequency(milliseconds_to_wait);

    const auto prof_start = metrics::readCpuTimer();

    TimeZoneBegin(for_loop);
    uint64_t counter = 0;
    for (uint64_t i = 0; i < 1000; ++i)
    {
        counter += i;
    }
    TimeZoneEnd(for_loop);

    printf_call();
    unique_ptr_call();
    write_file();
    remove_file();

    const auto prof_end = metrics::readCpuTimer();
    const auto prof_total = prof_end - prof_start;

    if (cpu_frequency)
    {
        printf("\nTotal time: %0.4fms (CPU: %.2fGHz)\n", metrics::cpuTimerToMilliseconds(prof_total, cpu_frequency), double(cpu_frequency) / 1000000000);
    }

    metrics::printScopes(prof_total);

    return 0;
}
