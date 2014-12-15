#include "TestCase.h"

#define _ENABLE_MEMORY_TAGGING_
#define _ENABLE_MEMORY_PROFILING_
#include <L3DMemory\SimpleArena.h>

struct S
{
    S()
    {
        Q = 3;
        P = 9;
    }

    int Q;
    double P;
};

int wmain()
{
    {
        L3DMemory::SimpleArena arena(64000);

        void* p = arena.Allocate(20, "void* p");
        float* f = static_cast<float*>(arena.AllocateAligned(sizeof(float), 8, "float* f"));
        float* f2 = static_cast<float*>(arena.AllocateAligned(sizeof(float), 8, "float* f2"));
        double* d = static_cast<double*>(arena.AllocateAligned(sizeof(double), 16, "double* d"));
        double* d2 = static_cast<double*>(arena.AllocateAligned(sizeof(double), 16, "double* d2"));

        arena.DumpAllTagsToOutput();
        arena.DumpProfileToOutput();
    }

    return 0;
}
