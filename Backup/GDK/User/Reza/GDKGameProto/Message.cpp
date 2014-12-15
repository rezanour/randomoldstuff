#include "StdAfx.h"
#include "Message.h"
#include "Executive.h"
#include "MemoryManager.h"

using GDK::Message;

Message::Message()
    : _body(nullptr)
{
}

Message::~Message()
{
    if (_body)
    {
        Executive::GetMemoryManager()->Free(_body);
    }
}

HRESULT Message::Create(GDK::MessageType type, uint16_t subType, size_t bodySize, Message** ppMessage)
{
    if (!ppMessage)
        return E_POINTER;

    HRESULT hr = S_OK;

    *ppMessage = nullptr;

    Message* msg = GDKNEW("Message") Message;
    hr = msg ? S_OK : E_OUTOFMEMORY;

    if (SUCCEEDED(hr))
    {
        msg->_body = Executive::GetMemoryManager()->Alloc(bodySize, "MessageBody");
        hr = (msg->_body) ? S_OK : E_OUTOFMEMORY;
    }

    if (SUCCEEDED(hr))
    {
        msg->_header.BodySize = bodySize;
        msg->_header.Type = type;
        msg->_header.SubType = subType;
        msg->_header.TimeSent = 0; // TODO: figure out what we want to do with time in the new world
    }

    if (SUCCEEDED(hr))
    {
        *ppMessage = msg;
    }
    else if (msg)
    {
        msg->Release();
    }

    return hr;
}

const GDK::MessageHeader* Message::GetHeader() const
{
    return &_header;
}

void* Message::GetBody()
{
    return _body;
}
