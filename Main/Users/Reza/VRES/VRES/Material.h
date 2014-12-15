#pragma once

namespace VRES
{
    class Material : NonCopyable<Material>
    {
    public:
        virtual ~Material();

        const std::string& Name() const { return _name; }

        void RenderModels(_In_count_(numModels) const Model* const* models, uint32_t numModels, _In_count_(numLights) const Light* const* lights, uint32_t numLights, const std::shared_ptr<HMD>& hmd);

    protected:
        Material(_In_z_ const char* name, _In_ Renderer* renderer, const ComPtr<ID3D11DeviceContext>& context);

        enum class ShaderStage
        {
            Vertex = 0,
            Pixel,
            Max
        };

        virtual void ApplyPipeline();
        virtual void RenderModels(const OutputDescription& output, _In_count_(numModels) const Model* const* models, uint32_t numModels, _In_count_(numLights) const Light* const* lights, uint32_t numLights) = 0;

        VRES::Renderer* Renderer() { return _renderer; }
        ComPtr<ID3D11Device>& Device() { return _device; }
        ComPtr<ID3D11DeviceContext>& Context() { return _context; }

        bool CreateVertexShader(VertexFormat vertexFormat, const void* byteCode, size_t byteCodeLength);
        bool CreatePixelShader(const void* byteCode, size_t byteCodeLength);

        bool CreateCB(ShaderStage stage, uint32_t index, uint32_t size);
        void* LockCB(ShaderStage stage, uint32_t index);
        void UnlockCB(ShaderStage stage, uint32_t index);

        void SetSampler(ShaderStage stage, uint32_t index, const ComPtr<ID3D11SamplerState>& sampler);

        void SetBlendState(_In_ const ComPtr<ID3D11BlendState>& blendState);
        void SetDepthStencilState(_In_ const ComPtr<ID3D11DepthStencilState>& depthStencilState);
        void SetRasterizerState(_In_ const ComPtr<ID3D11RasterizerState>& rasterizerState);

    private:
        static const uint32_t MaxCBs = 8;
        static const uint32_t MaxSamplers = 8;

        std::string _name;
        VRES::Renderer* _renderer;
        ComPtr<ID3D11Device> _device;
        ComPtr<ID3D11DeviceContext> _context;
        ComPtr<ID3D11InputLayout> _inputLayout;
        ComPtr<ID3D11VertexShader> _vertexShader;
        ComPtr<ID3D11PixelShader> _pixelShader;
        ComPtr<ID3D11BlendState> _blendState;
        ComPtr<ID3D11DepthStencilState> _depthStencilState;
        ComPtr<ID3D11RasterizerState> _rasterizerState;

        ID3D11Buffer* _cbs[(uint32_t)ShaderStage::Max][MaxCBs];
        ID3D11SamplerState* _samplers[(uint32_t)ShaderStage::Max][MaxSamplers];
    };
}
