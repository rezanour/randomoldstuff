#include "Platform.h"
#include "Scripting.h"

#define MAX_STACKS 16

static uint64_t* g_stacks[MAX_STACKS];
static uint64_t* g_stackEnds[MAX_STACKS];
static int g_numStacks;

int FindStackFromPointer(uint64_t* stackPointer)
{
    // find index for which stackPointer is between stack & stackEnd
    int i = 0;
    for (i = 0; i < g_numStacks; ++i)
    {
        if (stackPointer >= g_stacks[i] && 
            stackPointer < g_stackEnds[i])
        {
            return i;
        }
    }

    return MAX_STACKS;
}

HRESULT StackInit(size_t maxStackElements, uint64_t** stackPointer)
{
    if (!stackPointer)
    {
        return E_POINTER;
    }

    if (g_numStacks >= MAX_STACKS)
    {
        return HRESULT_FROM_WIN32(ERROR_OUT_OF_STRUCTURES);
    }

    int i = 0;
    for (i = 0; i < MAX_STACKS; ++i)
    {
        if (!g_stacks[i])
        {
            // found a free slot, let's use it
            void* stack = malloc(maxStackElements * sizeof(uint64_t) + sizeof(uint64_t*));
            if (!stack)
            {
                return E_OUTOFMEMORY;
            }

            g_stacks[i] = (uint64_t*)stack;
            g_stackEnds[i] = (uint64_t*)stack + maxStackElements;
            ++g_numStacks;
            return S_OK;
        }
    }

    return HRESULT_FROM_WIN32(ERROR_OUT_OF_STRUCTURES);
}

HRESULT StackDestroy(uint64_t* stackPointer)
{
    if (!stackPointer)
    {
        return E_POINTER;
    }

    int index = FindStackFromPointer(stackPointer);
    if (index == MAX_STACKS)
    {
        return E_INVALIDARG;
    }

    free(g_stacks[index]);
    g_stacks[index] = nullptr;
    g_stackEnds[index] = nullptr;
    --g_numStacks;

    return S_OK;
}
