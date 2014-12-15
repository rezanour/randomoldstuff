// Implementation of StreamHelper's template methods
#pragma once

#include <stde\types.h>
#include "Debug.h"

namespace CoreServices
{
    template <typename T>
    HRESULT StreamHelper::ReadValue(_Out_ T& value, _In_ size_t numBytes)
    {
        if (numBytes == 0)
            numBytes = sizeof(T);

        Assert(sizeof(T) >= numBytes);

        std::vector<byte> buffer(numBytes);
        unsigned long cbRead = 0;

        HRESULT hr = _spStream->Read(buffer.data(), static_cast<ulong>(numBytes), &cbRead);
        if (SUCCEEDED(hr))
        {
            if (cbRead != numBytes)
            {
                hr = E_UNEXPECTED;
            }
        }

        if (SUCCEEDED(hr))
        {
            if (memcpy_s(&value, sizeof(T), buffer.data(), numBytes) != 0)
            {
                hr = E_UNEXPECTED;
            }
        }

        return hr;
    }

    template <typename T>
    HRESULT StreamHelper::WriteValue(_In_ const T& value, _In_ size_t numBytes)
    {
        if (numBytes == 0)
            numBytes = sizeof(T);

        Assert(sizeof(T) >= numBytes);

        unsigned long cbWritten = 0;
        HRESULT hr = _spStream->Write(&value, static_cast<ulong>(numBytes), &cbWritten);
        if (SUCCEEDED(hr))
        {
            if (cbWritten != numBytes)
            {
                hr = E_UNEXPECTED;
            }
        }

        return hr;
    }
}

