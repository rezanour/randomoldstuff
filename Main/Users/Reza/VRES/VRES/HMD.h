#pragma once

#include "Renderer.h"

namespace VRES
{
    class HMD : NonCopyable<HMD>
    {
    public:
        static std::shared_ptr<HMD> Create(bool renderToHeadset);
        ~HMD();

        void GetWindowDimensions(_Out_ Point* location, _Out_ Size* size);

        bool Bind(Renderer* renderer);

        void BeginFrame();
        void EndFrame();

        void EnableHeadtracking(bool enable) { _headtrackingEnabled = enable; }
        bool HeadtrackingEnabled() const { return _headtrackingEnabled; }

        uint32_t NumOutputs() const { return _renderToHeadset ? _countof(_outputs) : 1; }
        const OutputDescription* Outputs() const { return _outputs; }

        XMFLOAT4 Orientation() const;

        void SetCameraTransform(const XMFLOAT3& position, const XMFLOAT4& orientation);

    private:
        HMD(bool renderToHeadset);

        bool Initialize();
        bool GraphicsInit(Renderer* renderer);

    private:
        ovrHmd _hmd;
        ovrHmdDesc _desc;
        ovrPosef _eyePose[2];
        ovrD3D11Texture _eyeTexture[2];
        ovrRecti _eyeRenderViewport[2];
        OutputDescription _outputs[2];
        ovrEyeRenderDesc _eyeRenderDesc[2];

        Renderer* _renderer;
        bool _renderToHeadset;
        bool _headtrackingEnabled;
        ComPtr<ID3D11DeviceContext> _context;
        ComPtr<IDXGISwapChain> _swapChain;
        ComPtr<ID3D11ShaderResourceView> _shaderResource;
        ComPtr<ID3D11RenderTargetView> _renderTarget;
        ComPtr<ID3D11DepthStencilView> _depthStencil;
        ComPtr<ID3D11RenderTargetView> _prevRTV;
        ComPtr<ID3D11DepthStencilView> _prevDSV;
    };
};
