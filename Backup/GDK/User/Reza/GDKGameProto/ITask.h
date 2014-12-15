#pragma once

#include "IRefCounted.h"

namespace GDK
{
    __interface IMessageQueue;
    __interface IObjectNamespace;

    enum TaskExecutionState
    {
        TES_Uninitialized = 0,
        TES_Running,
        TES_Suspended,
        TES_Completed,
        TES_Aborted
    };

    __interface ITask : public IRefCounted
    {
        // Properties
        uint32_t            GetTaskID() const;
        TaskExecutionState  GetExecutionState() const;

        // Messages
        IMessageQueue* GetMessageQueue();

        // Objects
        IObjectNamespace* GetObjectNamespace();
    };
} // GDK
