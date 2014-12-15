#pragma once

#include "MemoryObject.h"
#include "IMessageQueue.h"

namespace GDK
{
    class MessageQueue : protected MemoryObject, public IMessageQueue
    {
    public:
        // Construction/Destruction
        static HRESULT Create(MessageQueue** ppMsgQueue);

        // IRefCounted
        void AddRef();
        void Release();

        // IMessageQueue
        HRESULT PostMessage(MessageHeader* message);

    private:
        MessageQueue();
        ~MessageQueue();

        uint32_t _refCount;
    };
} // GDK
