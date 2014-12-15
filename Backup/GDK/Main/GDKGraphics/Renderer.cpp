#include "StdAfx.h"
#include "Renderer.h"
#include "StaticMeshVisualComponent.h"
#include "SimpleShader.h"
#include "DirectionalLightShader.h"
#include "LightCombineShader.h"

using namespace Lucid3D;
using namespace GDK;
using namespace CoreServices;

Renderer::Renderer()
{
    _clearColor[0] = 0.0f;
    _clearColor[1] = 0.0f;
    _clearColor[2] = 0.0f;
    _clearColor[3] = 1.0f;
}

Renderer::~Renderer()
{
}

GDK_IMETHODIMP Renderer::SetContentManager(_In_ IContentManager* pContentManager)
{
    _spContentManager.reset(pContentManager);
    return Geometry::CreateFullscreenQuad(this, GetContentManager(), _spQuad);
}

GDK_IMETHODIMP Renderer::Initialize(_In_ const GraphicsSettings& settings)
{
    HRESULT hr = S_OK;

    stde::ref_counted_ptr<IRendererView> spView;

    ID3D11RenderTargetView** rgbaRTs[] = { &_spDiffuseRT, &_spNormalsRT, &_spLightRT };
    ID3D11ShaderResourceView** rgbaSRVs[] = { &_spDiffuseSRV, &_spNormalsSRV, &_spLightSRV };

    stde::com_ptr<ID3D11Texture2D> spTexture;

    D3D11_TEXTURE2D_DESC texDesc;
    D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
    D3D11_SAMPLER_DESC samplerDesc;

    ZeroMemory(&texDesc, sizeof(texDesc));
    ZeroMemory(&rtDesc, sizeof(rtDesc));
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    ZeroMemory(&depthDesc, sizeof(depthDesc));
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));

    // Cache the settings for later
    _settings = settings;

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    DWORD flags = 0;

#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    CHECKHR(D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        &_spDevice,
        &_featureLevel,
        &_spImmediateContext));

    // Create default view
    CHECKHR(CreateView(settings.DefaultView, &spView));
    _spDefaultView.attach(reinterpret_cast<RendererView*>(spView.detach()));

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)settings.DefaultView.ScreenWidth;
    vp.Height = (FLOAT)settings.DefaultView.ScreenHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    _spImmediateContext->RSSetViewports( 1, &vp );

    // Create our depth & stencil buffer
    texDesc.Format = depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texDesc.Width = static_cast<uint>(settings.DefaultView.ScreenWidth);
    texDesc.Height = static_cast<uint>(settings.DefaultView.ScreenHeight);
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texDesc.MipLevels = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;

    depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    CHECKHR(_spDevice->CreateTexture2D(&texDesc, nullptr, &spTexture));
    CHECKHR(_spDevice->CreateDepthStencilView(spTexture, &depthDesc, &_spDepthStencil));
    spTexture = nullptr;

    // Create GBuffer's Diffuse & Normal buffers, and the Light buffer (all as RGBA)
    texDesc.Format = rtDesc.Format = srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    texDesc.Width = static_cast<uint>(settings.DefaultView.ScreenWidth);
    texDesc.Height = static_cast<uint>(settings.DefaultView.ScreenHeight);
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.MipLevels = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;

    rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    for (size_t i = 0; i < _countof(rgbaRTs); i++)
    {
        CHECKHR(_spDevice->CreateTexture2D(&texDesc, nullptr, &spTexture));
        CHECKHR(_spDevice->CreateRenderTargetView(spTexture, &rtDesc, rgbaRTs[i]));
        CHECKHR(_spDevice->CreateShaderResourceView(spTexture, &srvDesc, rgbaSRVs[i]));
        spTexture = nullptr;
    }

    // Create Combine buffer
    CHECKHR(_spDevice->CreateTexture2D(&texDesc, nullptr, &spTexture));
    CHECKHR(_spDevice->CreateRenderTargetView(spTexture, &rtDesc, &_spCombineRT));
    spTexture = nullptr;

    // Create the GBuffer's Depth Buffer
    texDesc.Format = rtDesc.Format = srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    CHECKHR(_spDevice->CreateTexture2D(&texDesc, nullptr, &spTexture));
    CHECKHR(_spDevice->CreateRenderTargetView(spTexture, &rtDesc, &_spDepthRT));
    CHECKHR(_spDevice->CreateShaderResourceView(spTexture, &srvDesc, &_spDepthSRV));
    spTexture = nullptr;

    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    CHECKHR(_spDevice->CreateSamplerState(&samplerDesc, &_spLinearSampler));

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    CHECKHR(_spDevice->CreateSamplerState(&samplerDesc, &_spPointSampler));

    // TODO: properly enumerate shaders
    {
        ShaderPtr spShader(new SimpleShader);
        CHECKHR(spShader->Initialize(this));
        _shaderMap[0] = spShader;

        spShader.attach(new DirectionalLightShader);
        CHECKHR(spShader->Initialize(this));
        _shaderMap[0xffffffff] = spShader;

        spShader.attach(new LightCombineShader);
        CHECKHR(spShader->Initialize(this));
        _shaderMap[0xfffffffe] = spShader;
    }

EXIT
    return hr;
}

GDK_IMETHODIMP Renderer::CreateView(_In_ const RendererViewSettings& viewSettings, _Deref_out_ IRendererView** ppView)
{
    HRESULT hr = S_OK;

    stde::ref_counted_ptr<RendererView> spView;

    ISNOTNULL(viewSettings.WindowIdentity, E_INVALIDARG);
    ISNOTNULL(ppView, E_POINTER);

    CHECKHR(RendererView::Create(viewSettings, _spImmediateContext, spView));

    _views.push_back(spView);
    *ppView = spView.detach();

EXIT
    if (FAILED(hr) && ppView)
    {
        *ppView = nullptr;
    }
    return hr;
}

GDK_IMETHODIMP Renderer::CreateScene(_Deref_out_ IRendererScene** ppScene)
{
    HRESULT hr = S_OK;

    stde::ref_counted_ptr<RendererScene> spScene;

    ISNOTNULL(ppScene, E_POINTER);

    spScene.attach(new RendererScene(this));
    ISNOTNULL(spScene, E_OUTOFMEMORY);

    _scenes.push_back(spScene);

    // by default, all scenes are associated with the default view
    CHECKHR(spScene->AssociateView(_spDefaultView));

    *ppScene = spScene.detach();

EXIT
    if (FAILED(hr) && ppScene)
    {
        *ppScene = nullptr;
    }
    return hr;
}

GDK_IMETHODIMP Renderer::GetViews(_Out_cap_(maxViews) IRendererView** views, _In_ size_t maxViews, _Out_ size_t* pNumViews)
{
    if (!views || !pNumViews)
    {
        return E_INVALIDARG;
    }

    *pNumViews = min(maxViews, _views.size());

    for (size_t i = 0; i < *pNumViews; i++)
    {
        views[i] = _views[i];
    }

    return S_OK;
}

GDK_IMETHODIMP Renderer::GetScenes(_Out_cap_(maxScenes) IRendererScene** scenes, _In_ size_t maxScenes, _Out_ size_t* pNumScenes)
{
    if (!scenes || !pNumScenes)
    {
        return E_INVALIDARG;
    }

    *pNumScenes = min(maxScenes, _scenes.size());

    for (size_t i = 0; i < *pNumScenes; i++)
    {
        scenes[i] = _scenes[i];
    }

    return S_OK;
}

ShaderPtr Renderer::GetShaderForMaterial(_In_ uint32 materialId)
{
    ShaderMap::iterator it = _shaderMap.find(materialId);
    Assert(it != _shaderMap.end());
    return it->second;
}

GDK_IMETHODIMP Renderer::Render()
{
    HRESULT hr = S_OK;

    ClearDepthStencil();
    ClearRenderTargets();

    for (size_t i = 0; i < _scenes.size(); i++)
    {
        // prepare for gbuffer (geometry) pass
        PrepareForGeometryPass();

        // Render Geometry to gbuffer
        CHECKHR(_scenes[i]->DrawGeometry(_spImmediateContext));

        // Prepare for lighting pass
        PrepareForLightingPass();

        // Render Lights
        CHECKHR(_scenes[i]->DrawLights(_spImmediateContext));

        CombineLighting();

        std::vector<IRendererView*> views(10);
        size_t numViews = 0;
        _scenes[i]->GetAssociatedViews(views.data(), views.size(), &numViews);
        for (size_t v = 0; v < numViews; v++)
        {
            RendererView* pView = static_cast<RendererView*>(views[v]);
            Resolve(pView->GetBackBuffer());
        }
    }

    for (size_t i = 0; i < _views.size(); i++)
    {
        CHECKHR(_views[i]->Present());
    }

EXIT
    return hr;
}

void Renderer::ClearDepthStencil()
{
    _spImmediateContext->ClearDepthStencilView(_spDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::ClearRenderTargets()
{
    static float normals[] = { 0, 0, 0, 0 };
    static float depth[] = {1, 1, 1, 1 };

    _spImmediateContext->ClearRenderTargetView(_spDiffuseRT, _clearColor);
    _spImmediateContext->ClearRenderTargetView(_spNormalsRT, normals);
    _spImmediateContext->ClearRenderTargetView(_spDepthRT, depth);

    static float ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    _spImmediateContext->ClearRenderTargetView(_spLightRT, ambient);
}

void Renderer::PrepareForGeometryPass()
{
    // Clear all shader resource views
    ID3D11ShaderResourceView* shaderResources[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
    _spImmediateContext->VSSetShaderResources(0, _countof(shaderResources), shaderResources);
    _spImmediateContext->PSSetShaderResources(0, _countof(shaderResources), shaderResources);

    // Bind the GBuffer render targets, with z-buffer
    ID3D11RenderTargetView* renderTargets[] = { _spDiffuseRT, _spNormalsRT, _spDepthRT };
    _spImmediateContext->OMSetRenderTargets(_countof(renderTargets), renderTargets, _spDepthStencil);
}

void Renderer::PrepareForLightingPass()
{
    // Set the light buffer render target, which unbinds the gbuffer render targets
    ID3D11RenderTargetView* renderTargets[] = { _spLightRT };
    _spImmediateContext->OMSetRenderTargets(_countof(renderTargets), renderTargets, nullptr);

    // Bind the normals and depths from the gbuffer as shader resource views
    ID3D11ShaderResourceView* shaderResources[] = { _spNormalsSRV, _spDepthSRV };
    _spImmediateContext->VSSetShaderResources(0, _countof(shaderResources), shaderResources);
    _spImmediateContext->PSSetShaderResources(0, _countof(shaderResources), shaderResources);

    // set up linear sampler for normals and point for depth
    ID3D11SamplerState* samplers[] = { _spLinearSampler, _spPointSampler };
    _spImmediateContext->VSSetSamplers(0, _countof(samplers), samplers);
    _spImmediateContext->PSSetSamplers(0, _countof(samplers), samplers);
}

void Renderer::CombineLighting()
{
    // Set combine surface as render target
    ID3D11RenderTargetView* renderTargets[] = { _spCombineRT };
    _spImmediateContext->OMSetRenderTargets(_countof(renderTargets), renderTargets, nullptr);

    // Bind the diffuse from the Gbuffer and the lighting output as shader resource views
    ID3D11ShaderResourceView* shaderResources[] = { _spDiffuseSRV, _spLightSRV };
    _spImmediateContext->VSSetShaderResources(0, _countof(shaderResources), shaderResources);
    _spImmediateContext->PSSetShaderResources(0, _countof(shaderResources), shaderResources);

    // set up linear for both diffuse & light
    ID3D11SamplerState* samplers[] = { _spLinearSampler, _spLinearSampler };
    _spImmediateContext->VSSetSamplers(0, _countof(samplers), samplers);
    _spImmediateContext->PSSetSamplers(0, _countof(samplers), samplers);

    ShaderPtr spShader = GetShaderForMaterial(0xfffffffe);
    MaterialPtr spMaterial;
    std::vector<RenderTaskPtr> tasks;
    tasks.push_back(std::make_shared<RenderTask>(Lucid3D::Matrix::Identity(), _spQuad, spMaterial, spShader));
    spShader->RenderBatch(_spImmediateContext, _scenes[0]->GetActiveCamera(), tasks);
}

void Renderer::Resolve(_In_ RenderTargetPtr& spTarget)
{
    stde::com_ptr<ID3D11Resource> spSource;
    stde::com_ptr<ID3D11Resource> spDest;

    _spCombineRT->GetResource(&spSource);
    spTarget->GetResource(&spDest);

    _spImmediateContext->CopyResource(spDest, spSource);
}
