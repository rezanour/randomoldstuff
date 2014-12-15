#pragma once

#include <stde\com_ptr.h>

namespace CoreServices
{
    // TODO: Build the rest of this object as needed
    class StreamHelper
    {
    public:
        explicit StreamHelper(_In_ stde::com_ptr<IStream> spStream)
            : _spStream(spStream)
        {
        }

        virtual ~StreamHelper()
        {
        }

        template <typename T>
        HRESULT ReadValue(_Out_ T& pValue, _In_ size_t numBytes = 0);

        template <typename T>
        HRESULT WriteValue(_In_ const T& value, _In_ size_t numBytes = 0);

    private:
        stde::com_ptr<IStream> _spStream;
    };
}

#include "StreamHelper.inl"
