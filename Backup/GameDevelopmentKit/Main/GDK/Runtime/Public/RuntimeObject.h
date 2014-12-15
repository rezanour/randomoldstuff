#pragma once

#include "Platform.h"
#include "NonCopyable.h"
#include "MemoryObject.h"

namespace GDK
{
    template <class Ty>
    class RuntimeObject : public NonCopyable, public MemoryObject, public std::enable_shared_from_this<Ty>
    {
    protected:
        RuntimeObject() {}
    };

} // GDK
