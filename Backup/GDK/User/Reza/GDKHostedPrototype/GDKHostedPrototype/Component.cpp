#include "StdAfx.h"

using namespace GDK;

Component::Component(_In_ Object* owner, _In_ ComponentType type)
    : _type(type), _owner(owner)
{
    assert(_owner);
    _owner->AddRef();
}

Component::~Component()
{
    SafeRelease(_owner);
}

ComponentType GDK_API Component::GetType() const
{
    return _type;
}

Object* GDK_API Component::GetOwner() const
{
    return _owner;
}
