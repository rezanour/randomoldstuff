#pragma once

#include <GDK\Transform.h>
#include "ResourceBase.h"
#include "Renderer.h"
#include "Material.h"
#include "Geometry.h"

#include <vector>

namespace Lucid3D
{
    class Model;
    typedef stde::ref_counted_ptr<Model> ModelPtr;

    class Model : stde::non_copyable, public GDK::RefCounted<GDK::IRefCounted>
    {
    public:
        static HRESULT Create(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _In_ uint64 contentId, _Out_ ModelPtr& spModel);

        GDK_IMETHOD Process(_Inout_ RenderMap& renderMap, _In_ GDK::Transform& transform);

    private:
        struct Mesh
        {
            GDK::Transform LocalTransform;
            GeometryPtr spGeometry;
            MaterialPtr spMaterial;
        };

        Model(_In_ const std::vector<Mesh>& meshes);

        std::vector<Mesh> _meshes;
    };
}

