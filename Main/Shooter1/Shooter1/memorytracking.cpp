#include "precomp.h"
#include "memorytracking.h"

static MemoryTracker g_MemoryTracker;

const char * const GetTagString(MemoryTag tag)
{
    switch (tag)
    {
    case MemoryTag::Graphics:       return "Graphics";
    case MemoryTag::GeoPool:        return "GeoPool";
    case MemoryTag::TexturePool:    return "TexturePool";
    case MemoryTag::AssetLoader:    return "AssetLoader";
    case MemoryTag::Draw2D:         return "Draw2D";
    case MemoryTag::Menu:           return "Menu";
    case MemoryTag::MenuItem:       return "MenuItem";
    case MemoryTag::Game:           return "Game";
    case MemoryTag::GamePlay:       return "GamePlay";
    case MemoryTag::GameObject:     return "GameObject";
    case MemoryTag::Material:       return "Material";
    case MemoryTag::Scene:          return "Scene";
    case MemoryTag::SceneNode:      return "SceneNode";
    case MemoryTag::BIHTriangle:    return "BIHTriangle";

    default:
        assert(false);
        return "INVALID";
    }
}

void MemoryTracker::AddAllocation(MemoryTag tag, size_t size)
{
    ++NumAllocations;
    TotalSize[(uint32_t)tag] += size;
}

void MemoryTracker::RemoveAllocation(MemoryTag tag, size_t size)
{
    TotalSize[(uint32_t)tag] -= size;
}

size_t MemoryTracker::StartNewFrame()
{
    size_t allocs = NumAllocations;
    NumAllocations = 0;
    return allocs;
}

void MemoryTracker::ReportMemUsage()
{
    DebugOut("\nMemoryStats:\n");
    for (uint32_t i = 0; i < _countof(TotalSize); ++i)
    {
        DebugOut("  %s - %I64u\n", GetTagString((MemoryTag)i), TotalSize[i]);
    }
}

void MemoryTrackerStartup()
{
    ZeroMemory(&g_MemoryTracker, sizeof(g_MemoryTracker));

    DebugOut("Memory Tracker started.\n");
}

void MemoryTrackerShutdown()
{
    DebugOut("Memory Tracker shutdown.\n");
}

MemoryTracker& GetMemoryTracker()
{
    return g_MemoryTracker;
}
