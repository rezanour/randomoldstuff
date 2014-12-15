#include "StdAfx.h"
#include "Executive.h"
#include "MemoryManager.h"
#include "ObjectNamespace.h"
#include "TaskManager.h"

using GDK::Executive;

Executive*  Executive::s_instance = nullptr;

Executive::Executive()
    :   _memMgr(nullptr),
        _taskMgr(nullptr),
        _msgQueue(nullptr),
        _globalObjs(nullptr),
        _config(nullptr)
{
}

Executive::~Executive()
{
}

HRESULT Executive::Startup()
{
    HRESULT hr = S_OK;

    // Ensure exec isn't already running
    if (s_instance)
    {
        OutputDebugString(L"Executive already running!\n");
        hr = HRESULT_FROM_WIN32(ERROR_OBJECT_ALREADY_EXISTS);
    }

    if (SUCCEEDED(hr))
    {
        s_instance = new Executive;
        hr = (s_instance ? S_OK : E_OUTOFMEMORY);
    }

    if (SUCCEEDED(hr))
    {
        hr = MemoryManager::Create(&s_instance->_memMgr);
    }

    if (SUCCEEDED(hr))
    {
        hr = ObjectNamespace::Create(&s_instance->_globalObjs);
    }

    if (SUCCEEDED(hr))
    {
        hr = TaskManager::Create(&s_instance->_taskMgr);
    }

    if (SUCCEEDED(hr))
    {
        // Hook atexit to verify that shutdown is called (otherwise, things like leak detection won't work)
        atexit(VerifyShutdown);
    }

    if (FAILED(hr))
    {
        delete s_instance;
        s_instance = nullptr;
    }

    return hr;
}

void Executive::Shutdown()
{
    Executive* exec = s_instance;

    // order matters somewhat. memory mgr should be last, so others can free anything they may have allocated there

    delete exec->_taskMgr;
    exec->_taskMgr = nullptr;

    exec->_globalObjs->Release();
    exec->_globalObjs = nullptr;

    delete exec->_memMgr;
    exec->_memMgr = nullptr;

    delete s_instance;
    s_instance = nullptr;
}

void __cdecl Executive::VerifyShutdown()
{
    if (s_instance)
    {
        OutputDebugString(L"** Executive::Shutdown() not called! This will prevent leak detection and other system services from working properly. **\n");
        assert("Executive::Shutdown() not called! This will prevent leak detection and other system services from working properly." && false);
    }
}

GDK::MemoryManager* Executive::GetMemoryManager()
{
    assert(s_instance);
    return s_instance->_memMgr;
}

GDK::ObjectNamespace* Executive::GetObjectNamespace()
{
    assert(s_instance);
    return s_instance->_globalObjs;
}

GDK::TaskManager* Executive::GetTaskManager()
{
    assert(s_instance);
    return s_instance->_taskMgr;
}
