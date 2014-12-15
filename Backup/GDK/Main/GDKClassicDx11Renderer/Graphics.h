#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _CLASSIC_DX11_GRAPHICS_H_
#define _CLASSIC_DX11_GRAPHICS_H_

namespace GDK
{
    extern "C"
    {
        struct IRenderer;

        HRESULT CreateRenderer(_In_ HWND hwnd, _Deref_out_ IRenderer** renderer);

    } // extern "C"
} // GDK

#endif // _CLASSIC_DX11_GRAPHICS_H_
