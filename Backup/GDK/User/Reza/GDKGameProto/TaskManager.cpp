#include "StdAfx.h"
#include "Executive.h"
#include "MemoryManager.h"
#include "TaskManager.h"
#include "Task.h"

using GDK::TaskManager;
using GDK::Task;

TaskManager::TaskManager()
    :   _maxThreads(8),
        _maxTasks(100),
        _numTasks(0),
        _maxScheduled(100),
        _numScheduled(0),
        _tasks(nullptr), 
        _schedule(nullptr),
        _latestFrameTime(0)
{
}

TaskManager::~TaskManager()
{
    CancelAllTasks();

    if (_tasks)
    {
        Executive::GetMemoryManager()->Free(_tasks);
        _tasks = nullptr;
    }

    if (_schedule)
    {
        Executive::GetMemoryManager()->Free(_schedule);
        _schedule = nullptr;
    }
}

HRESULT TaskManager::Create(TaskManager** ppTaskMgr)
{
    if (!ppTaskMgr)
        return E_POINTER;

    HRESULT hr = S_OK;
    *ppTaskMgr = nullptr;
    TaskManager* pTaskMgr = GDKNEW("TaskManager") TaskManager;
    hr = pTaskMgr ? S_OK : E_OUTOFMEMORY;

    if (SUCCEEDED(hr))
    {
        pTaskMgr->_tasks = static_cast<Task**>(Executive::GetMemoryManager()->Alloc(sizeof(Task*) * pTaskMgr->_maxTasks, "TaskManager::_tasks"));
        hr = pTaskMgr->_tasks ? S_OK : E_OUTOFMEMORY;

        if (SUCCEEDED(hr))
        {
            for (size_t i = 0; i < pTaskMgr->_maxTasks; ++i)
            {
                pTaskMgr->_tasks[i] = nullptr;
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        pTaskMgr->_schedule = static_cast<Task**>(Executive::GetMemoryManager()->Alloc(sizeof(Task*) * pTaskMgr->_maxScheduled, "TaskManager::_schedule"));
        hr = pTaskMgr->_schedule ? S_OK : E_OUTOFMEMORY;

        if (SUCCEEDED(hr))
        {
            for (size_t i = 0; i < pTaskMgr->_maxScheduled; ++i)
            {
                pTaskMgr->_schedule[i] = nullptr;
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        *ppTaskMgr = pTaskMgr;
    }
    else
    {
        delete pTaskMgr;
    }

    return hr;
}

HRESULT TaskManager::AddTask(Task* task)
{
    if (!task)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    uint32_t taskID = task->GetTaskID();

    // ensure we don't already have this task
    for (size_t i = 0; i < _numTasks; ++i)
    {
        if (_tasks[i]->GetTaskID() == taskID)
        {
            hr = HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
            break;
        }
    }

    if (SUCCEEDED(hr))
    {
        if (_numTasks < _maxTasks)
        {
            _tasks[_numTasks++] = task;
            task->AddRef();
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }

    return hr;
}

HRESULT TaskManager::SuspendTask(Task* task)
{
    UNREFERENCED_PARAMETER(task);
    return E_NOTIMPL;
}

HRESULT TaskManager::ResumeTask(Task* task)
{
    UNREFERENCED_PARAMETER(task);
    return E_NOTIMPL;
}

HRESULT TaskManager::CancelTask(Task* task)
{
    if (!task)
        return E_INVALIDARG;

    HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

    uint32_t taskID = task->GetTaskID();

    // first, remove from schedule
    for (size_t i = 0; i < _numScheduled; ++i)
    {
        if (_schedule[i]->GetTaskID() == taskID)
        {
            --_numScheduled;
            for (size_t j = i; j < _numScheduled; ++j)
            {
                _schedule[j] = _schedule[j + 1];
            }
            break;
        }
    }

    // next, remove the task
    for (size_t i = 0; i < _numTasks; ++i)
    {
        if (_tasks[i]->GetTaskID() == taskID)
        {
            hr = S_OK;
            _tasks[i]->Release();
            --_numTasks;
            for (size_t j = i; j < _numTasks; ++j)
            {
                _tasks[j] = _tasks[j + 1];
            }
            break;
        }
    }

    return hr;
}

void TaskManager::SuspendAllTasks()
{
}

void TaskManager::ResumeAllTasks()
{
}

void TaskManager::CancelAllTasks()
{
    for (size_t i = 0; i < _numScheduled; ++i)
    {
        _schedule[i] = nullptr;
    }

    for (size_t i = 0; i < _numTasks; ++i)
    {
        _tasks[i]->Release();
        _tasks[i] = nullptr;
    }
}

HRESULT TaskManager::RunOneFrame()
{
    // fill up schedule
    for (size_t i = 0; i < _numTasks; ++i)
    {
        // schedule everything for now until we have a real scheduler
        _schedule[_numScheduled++] = _tasks[i];
    }

    // execute schedule
    for (size_t i = 0; i < _numScheduled; ++i)
    {
        _schedule[i]->Run();
        _schedule[i] = nullptr; // remove from schedule
    }

    _numScheduled = 0;

    return S_OK;
}
