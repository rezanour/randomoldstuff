#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _CLASSIC_DX11_RENDERER_H_
#define _CLASSIC_DX11_RENDERER_H_

#include <stde\ref_ptr.h>

namespace GDK
{
    class Renderer : public RefCounted<IRenderer>
    {
    public:
        // Creation
        static stde::ref_ptr<Renderer> Create(_In_ HWND hwnd);

        // IRenderer
        GDK_IMETHOD_(void) AddScene(_In_ const stde::ref_ptr<IRendererScene>& scene);
        GDK_IMETHOD_(void) GetScenes(_Inout_ std::vector<stde::ref_ptr<IRendererScene>>& scenes) const;
        GDK_IMETHOD_(void) RenderAll();
        GDK_IMETHOD_(stde::ref_ptr<IMemoryStats>) GetMemoryStats();

    private:
        Renderer(_In_ HWND hwnd);
        ~Renderer();

        HWND _hwnd;
    };
} // GDK

#endif // _CLASSIC_DX11_RENDERER_H_
