#pragma once

#include "BaseObject.h"
#include "ITask.h"

namespace GDK
{
    class Task : public BaseObject<ITask>
    {
    public:
        // Construction/Destruction
        static HRESULT Create(Task** ppTask);

        // ITask
        uint32_t            GetTaskID() const;
        TaskExecutionState  GetExecutionState() const;
        IMessageQueue*      GetMessageQueue();
        IObjectNamespace*   GetObjectNamespace();

        // Execution
        HRESULT Initialize();
        HRESULT Cleanup();

        HRESULT Run();
        HRESULT Abort();

    private:
        Task();
        ~Task();

        static uint32_t s_nextTaskID;
        uint32_t        _taskID;
    };
} //GDK
