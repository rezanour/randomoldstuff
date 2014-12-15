#pragma once

#include "BaseObject.h"

namespace GDK {

class Message : public BaseObject
{
public:
    static _Check_return_ HRESULT Create(_In_ uint32_t messageType, _Deref_out_ Message** ppMessage);
    virtual ~Message();

    uint32_t GetID() const;
    uint32_t GetType() const;

private:
    Message(_In_ uint32_t messageType);

    static uint32_t s_nextID;
    uint32_t _messageID;
    uint32_t _messageType;
};

} // GDK
