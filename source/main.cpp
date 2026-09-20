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

void uniquePtrCall()
{
    TimeFunction();
    for (uint64_t i = 0; i < 1000; ++i)
    {
        std::unique_ptr unique_foo = std::make_unique<Foo>();
    }
}

void printfCall()
{
    TimeFunction();
    printf("Resulted counter: %llu\n", uint64_t(69));
}

void writeFile()
{
    TimeFunction();
    std::ofstream ofs("test_file.txt");
    ofs << "kek";
    ofs.close();
}

void removeFile()
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

    metrics::beginProfile(milliseconds_to_wait);

    TimeZoneBegin(for_loop);
    uint64_t counter = 0;
    for (uint64_t i = 0; i < 1000; ++i)
    {
        counter += i;
    }
    TimeZoneEnd(for_loop);

    printfCall();
    uniquePtrCall();
    writeFile();
    removeFile();

    metrics::endProfile();

    metrics::printStats();

    return 0;
}
