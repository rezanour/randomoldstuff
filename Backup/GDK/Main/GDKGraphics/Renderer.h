// Implements GDKGame's IRenderer interface. This is where all the fun happens! :)
#pragma once

#include <GDK\Graphics.h>
#include <map>

#include "GraphicsTypes.h"
#include "RendererView.h"
#include "RendererScene.h"

namespace Lucid3D
{
    class Renderer;
    typedef stde::ref_counted_ptr<Renderer> RendererPtr;

    class Renderer : stde::non_copyable, public GDK::RefCounted<GDK::IRenderer>
    {
    public:
        Renderer();
        ~Renderer();

        // ISubsystem
        GDK_IMETHOD_(GDK::SubsystemType) GetType() const { return GDK::SubsystemType::GraphicsSubsystem; }
        GDK_IMETHOD_(const char* const) GetName() const { return "Lucid3D DirectX Deferred Renderer"; }
        GDK_IMETHOD SetContentManager(_In_ GDK::IContentManager* pContentManager);

        // IRenderer
        GDK_IMETHOD Initialize(_In_ const GDK::GraphicsSettings& settings);

        GDK_IMETHOD CreateView(_In_ const GDK::RendererViewSettings& viewSettings, _Deref_out_ GDK::IRendererView** ppView);
        GDK_IMETHOD CreateScene(_Deref_out_ GDK::IRendererScene** ppScene);

        GDK_IMETHOD GetViews(_Out_cap_(maxViews) GDK::IRendererView** views, _In_ size_t maxViews, _Out_ size_t* pNumViews);
        GDK_IMETHOD GetScenes(_Out_cap_(maxScenes) GDK::IRendererScene** scenes, _In_ size_t maxScenes, _Out_ size_t* pNumScenes);

        GDK_IMETHOD Render();

        // Methods
        DevicePtr GetDevice() const { return _spDevice; }
        IContentManagerPtr GetContentManager() const { return _spContentManager; }
        ShaderPtr GetShaderForMaterial(_In_ uint32 materialId);
        const GDK::GraphicsSettings& GetSettings() const { return _settings; }

    private:
        void ClearDepthStencil();
        void ClearRenderTargets();

        void PrepareForGeometryPass();
        void PrepareForLightingPass();
        void CombineLighting();
        void Resolve(_In_ RenderTargetPtr& spTarget);

        std::vector<ViewPtr> _views;
        std::vector<ScenePtr> _scenes;
        ViewPtr _spDefaultView;

        // Cached copy of the settings used to initialize the renderer
        GDK::GraphicsSettings _settings;
        float _clearColor[4];
        ShaderMap _shaderMap;

        IContentManagerPtr _spContentManager;

        DevicePtr _spDevice;
        ContextPtr _spImmediateContext;
        D3D_FEATURE_LEVEL _featureLevel;
        DepthStencilPtr _spDepthStencil;

        stde::com_ptr<ID3D11SamplerState> _spLinearSampler;
        stde::com_ptr<ID3D11SamplerState> _spPointSampler;

        // GBuffer
        RenderTargetPtr _spDiffuseRT;
        RenderTargetPtr _spNormalsRT;
        RenderTargetPtr _spDepthRT;
        ShaderResourcePtr _spDiffuseSRV;
        ShaderResourcePtr _spNormalsSRV;
        ShaderResourcePtr _spDepthSRV;

        // Lights
        RenderTargetPtr _spLightRT;
        ShaderResourcePtr _spLightSRV;

        // Combine
        RenderTargetPtr _spCombineRT;

        GeometryPtr _spQuad;
    };
}

