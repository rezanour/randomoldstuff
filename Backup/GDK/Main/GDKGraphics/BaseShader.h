#pragma once

#include <stde\types.h>
#include <GDK\RefCounted.h>
#include <GDK\Transform.h>
#include <map>

#include "GraphicsTypes.h"
#include "CameraComponent.h"

namespace Lucid3D
{
    class Renderer;

    class BaseShader;
    typedef stde::ref_counted_ptr<BaseShader> ShaderPtr;

    typedef std::map<uint32, ShaderPtr> ShaderMap;

    struct RenderTask;

    class BaseShader : public GDK::RefCounted<GDK::IRefCounted>
    {
    public:
        virtual ~BaseShader();

        GDK_IMETHOD Initialize(_In_ Renderer* pRenderer) = 0;

        uint32 GetSupportedQuality(_In_ uint32 materialId) const;
        uint32 GetId() const { return _id; }

        // In order to create input layouts for D3D, we need this.
        // TODO: Find a better way to do input layouts... this is kind of poopy...
        stde::com_ptr<ID3DBlob> GetShaderCode() const { return _spCode; }

        // Render a group of tasks
        GDK_IMETHOD RenderBatch(_In_ ContextPtr& spContext, _In_ CameraPtr& spCamera, _In_ std::vector<std::shared_ptr<RenderTask>>& renderTasks) = 0;

    protected:
        BaseShader();

        void AddSupportedMaterial(_In_ uint32 materialId, _In_ uint32 quality);
        void SetShaderCode(_In_ stde::com_ptr<ID3DBlob> spCode);

    private:
        typedef std::map<uint32, uint32> QualityMap;
        QualityMap _supportedMaterials;

        stde::com_ptr<ID3DBlob> _spCode;
        uint32 _id;
        static uint32  s_nextId;
    };
}
