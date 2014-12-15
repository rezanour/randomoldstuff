#pragma once

#include "BaseRefCounted.h"

namespace GDK
{
    class ObjectNamespace;
    class MessageQueue;
    class MemoryManager;

    class Screen : public BaseRefCounted<IRefCounted>
    {
    public:
        // Construction/Destruction
        static HRESULT Create(ObjectNamespace* globals, Screen** ppScreen);

    private:
        Screen(ObjectNamespace* globals);
        ~Screen();

        MessageQueue*       _messageQueue;
        ObjectNamespace*    _globals;
        ObjectNamespace*    _objects;
        MemoryManager*      _memPool;
    };
} // GDK
