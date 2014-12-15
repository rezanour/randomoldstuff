#include "StdAfx.h"
#include "Message.h"

namespace GDK {

uint32_t Message::s_nextID = 0;

Message::Message(uint32_t messageType)
    : _messageID(s_nextID++), _messageType(messageType)
{
}

Message::~Message()
{
}

HRESULT Message::Create(uint32_t messageType, Message** ppMessage)
{
    if (!ppMessage)
        return E_POINTER;

    char messageName[30] = {0};
    sprintf_s(messageName, _countof(messageName), "Message, Type=%d", messageType);
    *ppMessage = new(messageName, "Message") Message(messageType);
    HRESULT hr = (*ppMessage) ? S_OK : E_OUTOFMEMORY;

    return hr;
}

uint32_t Message::GetID() const
{
    return _messageID;
}

uint32_t Message::GetType() const
{
    return _messageType;
}

} // GDK
