#pragma once

enum class TaskType
{
    None = 0,
    Texture,
    World,
};

struct ContentTask
{
    ContentTask(TaskType type, _In_z_ const wchar_t* source, _In_z_ const wchar_t* dest) :
        Type(type), Source(source), Dest(dest)
    {}

    std::wstring    Source;
    std::wstring    Dest;
    TaskType        Type;
};

struct TextureTask : ContentTask
{
    TextureTask(_In_z_ const wchar_t* source, _In_z_ const wchar_t* dest, bool resize, uint32_t width, uint32_t height, bool generateMips) :
        ContentTask(TaskType::Texture, source, dest), Resize(resize), Width(width), Height(height), GenerateMips(generateMips)
    {}

    bool        Resize;
    uint32_t    Width;
    uint32_t    Height;

    bool        GenerateMips;
};

struct WorldTask : ContentTask
{
    WorldTask(_In_z_ const wchar_t* source, _In_z_ const wchar_t* dest) :
        ContentTask(TaskType::World, source, dest)
    {}
};

class TaskManager
{
public:
    TaskManager();

    void AddTask(_In_ _Post_invalid_ ContentTask* task);
    void Run();

private:
    OutputManager* GetOutputManager();

    bool EvaluateTask(const std::unique_ptr<ContentTask>& task);

    void ProcessTask(const std::unique_ptr<ContentTask>& task);
    void ProcessTask(const TextureTask* task);
    void ProcessTask(const WorldTask* task);

private:
    std::atomic<bool> _running;
    CriticalSection _lock;
    std::unique_ptr<OutputManager> _outputManager;
    std::vector<std::unique_ptr<ContentTask>> _tasks;
};
