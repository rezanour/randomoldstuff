#include "precomp.h"
#include "vrsupport.h"

#ifndef ENABLE_VR

bool VREnabled()
{
    return false;
}

void VRInit()
{
}

void VRShutdown()
{
}

RECT VRGetLocation()
{
    RECT location = { 0, 0, 1280, 800 };
    return location;
}

_Use_decl_annotations_
void VRBeginFrame(ID3D11Device* device, IDXGISwapChain* swapChain)
{
    UNREFERENCED_PARAMETER(device);
    UNREFERENCED_PARAMETER(swapChain);
}

void VREndFrame()
{
}

_Use_decl_annotations_
void VRBeginEye(uint32_t eye, const XMFLOAT3& position, float yaw, D3D11_VIEWPORT* viewport, XMFLOAT4X4* view, XMFLOAT4X4* projection)
{
    UNREFERENCED_PARAMETER(eye);
    UNREFERENCED_PARAMETER(position);
    UNREFERENCED_PARAMETER(yaw);
    ZeroMemory(viewport, sizeof(*viewport));
    ZeroMemory(view, sizeof(*view));
    ZeroMemory(projection, sizeof(*projection));
}

void VREndEye()
{
}

XMFLOAT4 VRGetOrientation()
{
    return XMFLOAT4(0, 0, 0, 1);
}

#else

using namespace OVR;

static ovrHmd               g_HMD;
static ovrHmdDesc           g_HMDDesc;
static Sizei                g_RenderTargetSize;
static ovrEyeRenderDesc     g_EyeRenderDesc[2];
static ovrRecti             g_EyeRenderViewport[2];
static ovrD3D11Texture      g_EyeTexture[2];
static ovrFrameTiming       g_FrameTiming;
static Transformf           g_MovePose;
static ovrEyeType           g_CurrentEye;
static ovrPosef             g_EyeRenderPose[2];

static ComPtr<ID3D11Device>                 g_Device;
static ComPtr<ID3D11DeviceContext>          g_Context;
static ComPtr<IDXGISwapChain>               g_SwapChain;
static ComPtr<ID3D11Texture2D>              g_RenderTarget;
static ComPtr<ID3D11RenderTargetView>       g_RenderTargetView;
static ComPtr<ID3D11ShaderResourceView>     g_ShaderResourceView;
static ComPtr<ID3D11DepthStencilView>       g_DepthStencilView;

static ComPtr<ID3D11RenderTargetView>       g_PrevRTV;
static ComPtr<ID3D11DepthStencilView>       g_PrevDSV;

static void GraphicsInit();

bool VREnabled()
{
    return true;
}

void VRInit()
{
    ovr_Initialize();

    g_HMD = ovrHmd_Create(0);
    if (!g_HMD)
    {
        throw std::exception();
    }

    ovrHmd_GetDesc(g_HMD, &g_HMDDesc);
    if (g_HMDDesc.DisplayDeviceName[0] == 0)
    {
        throw std::exception();
    }

    ovrHmd_SetEnabledCaps(g_HMD, ovrHmdCap_LowPersistence);

    ovrHmd_StartSensor(g_HMD, ovrSensorCap_Orientation |
                              ovrSensorCap_YawCorrection |
                              ovrSensorCap_Position, 0);
}

void VRShutdown()
{
    ovrHmd_Destroy(g_HMD);

    ovr_Shutdown();
}

RECT VRGetLocation()
{
    Recti location(g_HMDDesc.WindowsPos, g_HMDDesc.Resolution);
    RECT loc = { location.x, location.y, location.x + location.w, location.y + location.h };
    return loc;
}

_Use_decl_annotations_
void VRBeginFrame(ID3D11Device* device, IDXGISwapChain* swapChain)
{
    if (!g_Device)
    {
        g_Device = device;
        g_Device->GetImmediateContext(&g_Context);
        g_SwapChain = swapChain;

        GraphicsInit();
    }

    g_FrameTiming = ovrHmd_BeginFrame(g_HMD, 0);
    g_MovePose = ovrHmd_GetSensorState(g_HMD, g_FrameTiming.ScanoutMidpointSeconds).Predicted.Pose;

    g_Context->OMSetRenderTargets(1, g_PrevRTV.GetAddressOf(), g_PrevDSV.Get());

    g_Context->OMSetRenderTargets(1, g_RenderTargetView.GetAddressOf(), g_DepthStencilView.Get());

    static const float clearColor[] = { 0, 0, 1, 1 };
    g_Context->ClearRenderTargetView(g_RenderTargetView.Get(), clearColor);
    g_Context->ClearDepthStencilView(g_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void VREndFrame()
{
    ovrHmd_EndFrame(g_HMD);
}

_Use_decl_annotations_
void VRBeginEye(uint32_t eye, const XMFLOAT3& position, float yaw, D3D11_VIEWPORT* viewport, XMFLOAT4X4* view, XMFLOAT4X4* projection)
{
    g_CurrentEye = g_HMDDesc.EyeRenderOrder[eye];
    g_EyeRenderPose[g_CurrentEye] = ovrHmd_BeginEyeRender(g_HMD, g_CurrentEye);

    ovrQuatf orientation = g_EyeRenderPose[g_CurrentEye].Orientation;
    orientation.y *= -1;
    orientation.w *= -1;

    Matrix4f rollPitchYaw = Matrix4f::RotationY(yaw) * Matrix4f(orientation);
    Vector3f up = rollPitchYaw.Transform(Vector3f(0, 1, 0));
    Vector3f forward = rollPitchYaw.Transform(Vector3f(0, 0, 1));
    Vector3f shiftedEyePos = Vector3f(position.x, position.y, position.z) + rollPitchYaw.Transform(g_EyeRenderPose[g_CurrentEye].Position);

    Matrix4f theView = Matrix4f::LookAtRH(shiftedEyePos,
                                          shiftedEyePos + forward,
                                          up);

    theView =  Matrix4f::Translation(g_EyeRenderDesc[eye].ViewAdjust) * theView;
    XMStoreFloat4x4(view, XMMatrixTranspose(XMLoadFloat4x4((XMFLOAT4X4*)theView.M)));

    Matrix4f theProj = ovrMatrix4f_Projection(g_EyeRenderDesc[g_CurrentEye].Fov, 0.01f, 10000.0f, true);
    XMStoreFloat4x4(projection, XMMatrixTranspose(XMLoadFloat4x4((XMFLOAT4X4*)theProj.M)));

    viewport->TopLeftX = (float)g_EyeRenderViewport[g_CurrentEye].Pos.x;
    viewport->TopLeftY = (float)g_EyeRenderViewport[g_CurrentEye].Pos.y;
    viewport->Width = (float)g_EyeRenderViewport[g_CurrentEye].Size.w;
    viewport->Height = (float)g_EyeRenderViewport[g_CurrentEye].Size.h;
    viewport->MaxDepth = 1.0f;
}

void VREndEye()
{
    ovrHmd_EndEyeRender(g_HMD, g_CurrentEye, g_EyeRenderPose[g_CurrentEye], &g_EyeTexture[g_CurrentEye].Texture);
}

XMFLOAT4 VRGetOrientation()
{
    ovrQuatf orientation = ovrHmd_GetSensorState(g_HMD, 0).Recorded.Pose.Orientation;
    return XMFLOAT4(orientation.x, -orientation.y, orientation.z, -orientation.w);
}

//
// Private
//

void GraphicsInit()
{
    // Configure stereo
    Sizei size0 = ovrHmd_GetFovTextureSize(g_HMD, ovrEye_Left, g_HMDDesc.DefaultEyeFov[0], 1.0f);
    Sizei size1 = ovrHmd_GetFovTextureSize(g_HMD, ovrEye_Right, g_HMDDesc.DefaultEyeFov[1], 1.0f);
    g_RenderTargetSize.w = size0.w + size1.w;
    g_RenderTargetSize.h = std::max(size0.h, size1.h);

    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 1;
    td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.Height = g_RenderTargetSize.h;
    td.Width = g_RenderTargetSize.w;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;

    HRESULT hr = g_Device->CreateTexture2D(&td, nullptr, &g_RenderTarget);
    if (FAILED(hr)) throw std::exception();

    hr = g_Device->CreateRenderTargetView(g_RenderTarget.Get(), nullptr, &g_RenderTargetView);
    if (FAILED(hr)) throw std::exception();

    hr = g_Device->CreateShaderResourceView(g_RenderTarget.Get(), nullptr, &g_ShaderResourceView);
    if (FAILED(hr)) throw std::exception();

    ovrFovPort eyeFov[2] = { g_HMDDesc.DefaultEyeFov[0], g_HMDDesc.DefaultEyeFov[1] };

    g_EyeRenderViewport[0].Pos = Vector2i(0, 0);
    g_EyeRenderViewport[0].Size = Sizei(g_RenderTargetSize.w / 2, g_RenderTargetSize.h);
    g_EyeRenderViewport[1].Pos = Vector2i((g_RenderTargetSize.w + 1) / 2, 0);
    g_EyeRenderViewport[1].Size = g_EyeRenderViewport[0].Size;

    g_EyeTexture[0].D3D11.Header.API = ovrRenderAPI_D3D11;
    g_EyeTexture[0].D3D11.Header.TextureSize = g_RenderTargetSize;
    g_EyeTexture[0].D3D11.Header.RenderViewport = g_EyeRenderViewport[0];
    g_EyeTexture[0].D3D11.pTexture = g_RenderTarget.Get();
    g_EyeTexture[0].D3D11.pSRView = g_ShaderResourceView.Get();

    g_EyeTexture[1] = g_EyeTexture[0];
    g_EyeTexture[1].D3D11.Header.RenderViewport = g_EyeRenderViewport[1];

    td.Format = DXGI_FORMAT_D32_FLOAT;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ComPtr<ID3D11Texture2D> depthTexture;
    hr = g_Device->CreateTexture2D(&td, nullptr, &depthTexture);
    if (FAILED(hr)) throw std::exception();

    hr = g_Device->CreateDepthStencilView(depthTexture.Get(), nullptr, &g_DepthStencilView);
    if (FAILED(hr)) throw std::exception();

    g_Context->OMGetRenderTargets(1, &g_PrevRTV, &g_PrevDSV);

    ovrD3D11Config d3d11Config;
    d3d11Config.D3D11.Header.API = ovrRenderAPI_D3D11;
    d3d11Config.D3D11.Header.RTSize = g_HMDDesc.Resolution;
    d3d11Config.D3D11.Header.Multisample = 1;
    d3d11Config.D3D11.pDevice = g_Device.Get();
    d3d11Config.D3D11.pDeviceContext = g_Context.Get();
    d3d11Config.D3D11.pSwapChain = g_SwapChain.Get();
    d3d11Config.D3D11.pBackBufferRT = g_PrevRTV.Get();

    if (!ovrHmd_ConfigureRendering(g_HMD, &d3d11Config.Config,
                                   ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp,
                                   eyeFov, g_EyeRenderDesc))
    {
        throw std::exception();
    }
}

#endif
