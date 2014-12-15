#pragma once

#include <GDK\Graphics.h>
#include <vector>

#include "GraphicsTypes.h"

namespace Lucid3D
{
    class RendererView;
    typedef stde::ref_counted_ptr<RendererView> ViewPtr;

    class RendererView : stde::non_copyable, public GDK::RefCounted<GDK::IRendererView>
    {
    public:
        static HRESULT Create(_In_ const GDK::RendererViewSettings& viewSettings, _In_ ContextPtr& spContext, _Out_ ViewPtr& spView);

        // IRendererView
        GDK_IMETHOD_(const void*) GetWindowIdentity() const;

        GDK_IMETHOD_(bool) IsFullScreen() const;
        GDK_IMETHOD SetFullScreen(_In_ bool fullScreen);

        GDK_IMETHOD GetSize(_Out_ size_t* pWidth, _Out_ size_t* pHeight) const;
        GDK_IMETHOD SetSize(_In_ size_t width, _In_ size_t height);

        // Methods
        RenderTargetPtr GetBackBuffer() const;

        HRESULT Present();

    private:
        RendererView();
        HRESULT Initialize(_In_ const GDK::RendererViewSettings& viewSettings, _In_ ContextPtr& spContext);

        HWND _hwnd;
        GDK::RendererViewSettings _settings;

        SwapChainPtr _spSwapChain;
        ContextPtr _spContext;
        RenderTargetPtr _spBackBuffer;
    };
}
