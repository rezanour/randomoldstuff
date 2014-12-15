#include "StdAfx.h"
#include "Task.h"
#include "TypeObject.h"

using GDK::Task;

uint32_t Task::s_nextTaskID = 0;

Task::Task()
    : _taskID(s_nextTaskID++)
{
}

Task::~Task()
{
}

HRESULT Task::Create(Task** ppTask)
{
    if (!ppTask)
        return E_POINTER;

    HRESULT hr = S_OK;

    *ppTask = nullptr;
    Task* task = new("Task") Task;
    hr = task ? S_OK : E_OUTOFMEMORY;

    if (SUCCEEDED(hr))
    {
        TypeObject* pType = nullptr;
        hr = TypeObject::Create("Task", &pType);
        if (SUCCEEDED(hr))
        {
            task->SetType(pType);
            pType->Release();
        }
    }

    if (SUCCEEDED(hr))
    {
        *ppTask = task;
    }
    else if (task)
    {
        task->Release();
    }

    return hr;
}

uint32_t Task::GetTaskID() const
{
    return _taskID;
}

GDK::TaskExecutionState Task::GetExecutionState() const
{
    return TES_Uninitialized;
}

GDK::IMessageQueue* Task::GetMessageQueue()
{
    return nullptr;
}

GDK::IObjectNamespace* Task::GetObjectNamespace()
{
    return nullptr;
}

HRESULT Task::Initialize()
{
    return E_NOTIMPL;
}

HRESULT Task::Cleanup()
{
    return E_NOTIMPL;
}

HRESULT Task::Run()
{
    return E_NOTIMPL;
}

HRESULT Task::Abort()
{
    return E_NOTIMPL;
}
