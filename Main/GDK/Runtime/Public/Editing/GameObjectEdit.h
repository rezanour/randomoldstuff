#pragma once

#include <Platform.h>

namespace GDK
{
    class Transform;

    struct IGameObjectEdit
    {
        virtual const std::wstring& GetClassName() const = 0;

        virtual const Transform& GetTransform() const = 0;
        virtual Transform& GetTransform() = 0;
    };
}
