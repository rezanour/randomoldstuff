#pragma once

#ifdef ENABLE_VR

#include <OVR.h>

#define OVR_D3D_VERSION 11
#include <..\Src\OVR_CAPI_D3D.h>

#ifdef _DEBUG
#pragma comment(lib, "VS2013\\libovrd.lib")
#else
#pragma comment(lib, "VS2013\\libovr.lib")
#endif

#endif

bool VREnabled();

void VRInit();
void VRShutdown();

// Used to compute where & how big the output window should be to position it
// inside the HMD
RECT VRGetLocation();

// Bracket frames with these
void VRBeginFrame(_In_ ID3D11Device* device, _In_ IDXGISwapChain* swapChain);
void VREndFrame(); // This also presents for caller, so don't double present!

void VRBeginEye(uint32_t eye, const XMFLOAT3& position, float yaw, _Out_ D3D11_VIEWPORT* viewport, _Out_ XMFLOAT4X4* view, _Out_ XMFLOAT4X4* projection);
void VREndEye();

XMFLOAT4 VRGetOrientation();
