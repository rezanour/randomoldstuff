#pragma once

namespace GDK
{
    __interface IRefCounted
    {
        void AddRef();
        void Release();
    };
} // GDK
