#include "StdAfx.h"
#include "Executive.h"
#include "MemoryManager.h"
#include "ObjectManager.h"

namespace GDK {

Executive* Executive::s_instance = nullptr;

Executive::Executive()
    :   _memMgr(nullptr), 
        _objMgr(nullptr), 
        _devMgr(nullptr),
        _screenMgr(nullptr),
        _taskMgr(nullptr),
        _timerMgr(nullptr),
        _msgQueue(nullptr)
{
}

Executive::~Executive()
{
    // If any of our managers are non-null, we haven't been 
    // properly shutdown.
    if (_msgQueue)
    {
        // TODO: Raise an error
        OutputDebugString(L"Executive not properly shutdown!\n");
        Shutdown();
    }
}

HRESULT Executive::Startup()
{
    // Ensure exec isn't already running
    if (GetExecutive())
    {
        // TODO: Raise an error
        OutputDebugString(L"Executive already started!\n");
    }

    HRESULT hr = S_OK;

    s_instance = new Executive;
    hr = (s_instance ? S_OK : E_OUTOFMEMORY);

    if (SUCCEEDED(hr))
    {
        // TODO: actually startup
        hr = MemoryManager::Create(&s_instance->_memMgr);
    }

    if (SUCCEEDED(hr))
    {
        hr = ObjectManager::Create(&s_instance->_objMgr);
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
    Executive* exec = GetExecutive();

    // order matters, memory mgr should be last, so others can free what they have
    delete exec->_objMgr;
    delete exec->_memMgr;

    exec->_memMgr = nullptr;
    exec->_objMgr = nullptr;
    exec->_devMgr = nullptr;
    exec->_screenMgr = nullptr;
    exec->_taskMgr = nullptr;
    exec->_timerMgr = nullptr;
    exec->_msgQueue = nullptr;
}

Executive* Executive::GetExecutive()
{
    return s_instance;
}

MemoryManager* Executive::GetMemoryManager()
{
    return _memMgr;
}

ObjectManager* Executive::GetObjectManager()
{
    return _objMgr;
}

DeviceManager* Executive::GetDeviceManager()
{
    return _devMgr;
}

ScreenManager* Executive::GetScreenManager()
{
    return _screenMgr;
}

TaskManager* Executive::GetTaskManager()
{
    return _taskMgr;
}

TimerManager* Executive::GetTimerManager()
{
    return _timerMgr;
}

MessageQueue* Executive::GetMessageQueue()
{
    return _msgQueue;
}

} // GDK
