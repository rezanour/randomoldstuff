#include "StdAfx.h"
#include "MessageQueue.h"

using GDK::MessageQueue;

MessageQueue::MessageQueue()
    : _refCount(1)
{
}

MessageQueue::~MessageQueue()
{
}

HRESULT MessageQueue::Create(MessageQueue** ppMsgQueue)
{
    if (!ppMsgQueue)
        return E_POINTER;

    *ppMsgQueue = GDKNEW("MessageQueue") MessageQueue;
    return (*ppMsgQueue) ? S_OK : E_OUTOFMEMORY;
}

void MessageQueue::AddRef()
{
    InterlockedIncrement(&_refCount);
}

void MessageQueue::Release()
{
    if (InterlockedDecrement(&_refCount) == 0)
    {
        delete this;
    }
}

HRESULT MessageQueue::PostMessage(GDK::MessageHeader* message)
{
    UNREFERENCED_PARAMETER(message);
    return E_NOTIMPL;
}
