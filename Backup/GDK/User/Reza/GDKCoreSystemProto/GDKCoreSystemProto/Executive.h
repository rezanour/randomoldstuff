#pragma once

namespace GDK {

class MemoryManager;
class ObjectManager;
class DeviceManager;
class ScreenManager;
class TaskManager;
class TimerManager;
class MessageQueue;

class Executive
{
public:
    static _Check_return_ HRESULT   Startup();
    static void                     Shutdown();
    static _Ret_ Executive*         GetExecutive();

    _Ret_ MemoryManager*            GetMemoryManager();
    _Ret_ ObjectManager*            GetObjectManager();
    _Ret_ DeviceManager*            GetDeviceManager();
    _Ret_ ScreenManager*            GetScreenManager();
    _Ret_ TaskManager*              GetTaskManager();
    _Ret_ TimerManager*             GetTimerManager();
    _Ret_ MessageQueue*             GetMessageQueue();

private:
    Executive();
    ~Executive();

    static Executive*   s_instance;
    MemoryManager*      _memMgr;
    ObjectManager*      _objMgr;
    DeviceManager*      _devMgr;
    ScreenManager*      _screenMgr;
    TaskManager*        _taskMgr;
    TimerManager*       _timerMgr;
    MessageQueue*       _msgQueue;
};

} // GDK
