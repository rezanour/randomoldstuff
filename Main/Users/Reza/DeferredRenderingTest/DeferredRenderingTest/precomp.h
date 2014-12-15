#pragma once

#include <Windows.h>
#include <d3d11.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <wrl.h>
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <stdexcpt.h>

#include <memory>
#include <string>
#include <vector>
#include <exception>

#include <ppl.h>
#include <ppltasks.h>

template <typename Interface>
inline void SafeRelease(Interface*& p)
{
    if (p != nullptr)
    {
        p->Release();
        p = nullptr;
    }
}
