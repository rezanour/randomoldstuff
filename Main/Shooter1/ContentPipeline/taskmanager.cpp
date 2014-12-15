#include "precomp.h"

TaskManager::TaskManager() :
    _running(false), _outputManager(OutputManager::Create())
{
}

_Use_decl_annotations_
void TaskManager::AddTask(ContentTask* task)
{
    auto lock = _lock.Lock();
    _tasks.push_back(std::unique_ptr<ContentTask>(task));
}

void TaskManager::Run()
{
    bool expected = false;
    if (!_running.compare_exchange_strong(expected, true))
    {
        Error("TaskManager already running.");
    }

    std::vector<std::unique_ptr<ContentTask>> tasks;

    for (;;)
    {
        tasks.clear();

        {
            auto lock = _lock.Lock();
            _tasks.swap(tasks);
        }

        if (tasks.empty())
        {
            break;
        }

        concurrency::parallel_for((size_t)0, tasks.size(), [&](size_t i)
        {
            ProcessTask(tasks[i]);
        });
    }
}

OutputManager* TaskManager::GetOutputManager()
{
    return _outputManager.get();
}

bool TaskManager::EvaluateTask(const std::unique_ptr<ContentTask>& task)
{
    if (GetFileAttributes((GetConfig()->DestRoot + task->Dest).c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        // destination doesn't exit. Always run task
        return true;
    }

    FILETIME sourceWriteTime = {};
    FILETIME destWriteTime = {};

    {
        FileHandle source(CreateFile((GetConfig()->SourceRoot + task->Source).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
        CHECKGLE(source.IsValid());

        CHECKGLE(GetFileTime(source.Get(), nullptr, nullptr, &sourceWriteTime));
    }

    {
        FileHandle dest(CreateFile((GetConfig()->DestRoot + task->Dest).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
        CHECKGLE(dest.IsValid());

        CHECKGLE(GetFileTime(dest.Get(), nullptr, nullptr, &destWriteTime));
    }

    if (CompareFileTime(&sourceWriteTime, &destWriteTime) > 0)
    {
        // source asset has been updated since the last time we generated the output, run task
        return true;
    }

    // No need to run task
    return false;
}

void TaskManager::ProcessTask(const std::unique_ptr<ContentTask>& task)
{
    // Do we even need to run this task?
    if (!EvaluateTask(task))
    {
        return;
    }

    switch (task->Type)
    {
    case TaskType::Texture:
        ProcessTask(static_cast<const TextureTask*>(task.get()));
        break;

    case TaskType::World:
        ProcessTask(static_cast<const WorldTask*>(task.get()));
        break;

    default:
        Error("Invalid task type!");
    }
}

void TaskManager::ProcessTask(const TextureTask* task)
{
    std::unique_ptr<TextureContent> texture(TextureContent::Load((GetConfig()->SourceRoot + task->Source).c_str()));

    if (task->Resize)
    {
        texture->ResizeImage(task->Width, task->Height);
    }

    if (task->GenerateMips)
    {
        texture->GenerateMips();
    }

    auto output = GetOutputManager()->GetOutput(task->Dest.c_str());
    texture->Save(output);
}

void TaskManager::ProcessTask(const WorldTask* task)
{
    //
    // TODO: Implement this for real. Right now it just copies source to dest
    //
    FileHandle input(CreateFile((GetConfig()->SourceRoot + task->Source).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    CHECKGLE(input.IsValid());

    uint32_t size = GetFileSize(input.Get(), nullptr);
    std::unique_ptr<BYTE[]> buffer(new BYTE[size]);

    DWORD read = 0;
    CHECKGLE(ReadFile(input.Get(), buffer.get(), size, &read, nullptr));
    CHECKTRUE(read == size);

    auto output = GetOutputManager()->GetOutput(task->Dest.c_str());
    output.Write(buffer.get(), size);
}
