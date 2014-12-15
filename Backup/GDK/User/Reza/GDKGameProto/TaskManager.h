#pragma once

#include "MemoryObject.h"

namespace GDK
{
    class Task;

    class TaskManager : public MemoryObject
    {
    public:
        // Construction/Destruction
        static HRESULT Create(TaskManager** ppTaskMgr);
        ~TaskManager();

        // Manage tasks
        HRESULT AddTask(Task* task);

        HRESULT SuspendTask(Task* task);
        HRESULT ResumeTask(Task* task);
        HRESULT CancelTask(Task* task);

        void SuspendAllTasks();
        void ResumeAllTasks();
        void CancelAllTasks();

        // Updates
        HRESULT RunOneFrame();

    private:
        TaskManager();

        // scheduling
        uint16_t _maxThreads;
        uint16_t _maxTasks;
        uint16_t _numTasks;
        uint16_t _maxScheduled;
        uint16_t _numScheduled;

        Task** _tasks;
        Task** _schedule;

        // time management
        uint64_t _latestFrameTime;
    };
} // GDK
