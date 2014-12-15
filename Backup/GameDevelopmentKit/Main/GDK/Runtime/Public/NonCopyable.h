#pragma once

#include "Platform.h"

namespace GDK
{
    class NonCopyable
    {
    protected:
        NonCopyable() {}

    private:
        NonCopyable(_In_ const NonCopyable&);
        NonCopyable& operator=(_In_ const NonCopyable&);
    };
}
