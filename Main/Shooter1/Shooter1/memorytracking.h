#pragma once

enum class MemoryTag
{
    Graphics = 0,
    TexturePool,
    GeoPool,
    Draw2D,
    Menu,
    MenuItem,
    GamePlay,
    GameObject,
    Game,
    AssetLoader,
    Material,
    Scene,
    SceneNode,
    BIHTriangle,
    Max
};

const char * const GetTagString(MemoryTag tag);

struct MemoryTracker
{
    size_t NumAllocations;
    size_t TotalSize[(uint32_t)MemoryTag::Max];

    void AddAllocation(MemoryTag tag, size_t size);
    void RemoveAllocation(MemoryTag tag, size_t size);

    // Returns the number of allocations made since last call
    size_t StartNewFrame();
    void ReportMemUsage();
};

void MemoryTrackerStartup();
void MemoryTrackerShutdown();

MemoryTracker& GetMemoryTracker();
