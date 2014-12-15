#pragma once

namespace GDK
{
    class MemoryManager;
    class TaskManager;
    class MessageQueue;
    class ObjectNamespace;
    class Configuration;

    class Executive
    {
    public:
        static HRESULT          Startup();
        static void             Shutdown();

        static MemoryManager*   GetMemoryManager();
        static ObjectNamespace* GetObjectNamespace();
        static TaskManager*     GetTaskManager();

    private:
        Executive();
        ~Executive();

        static void __cdecl     VerifyShutdown();

        static Executive*       s_instance;
        MemoryManager*          _memMgr;
        TaskManager*            _taskMgr;
        MessageQueue*           _msgQueue;
        ObjectNamespace*        _globalObjs;
        Configuration*          _config;
    };
} // GDK
