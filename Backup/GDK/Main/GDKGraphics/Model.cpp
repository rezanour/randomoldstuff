#include "StdAfx.h"
#include "Model.h"
#include <CoreServices\Configuration.h>

using namespace Lucid3D;
using namespace GDK;
using namespace DirectX;
using namespace CoreServices;

Model::Model(_In_ const std::vector<Mesh>& meshes)
    : _meshes(meshes)
{
}

HRESULT Model::Create(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _In_ uint64 contentId, _Out_ ModelPtr& spModel)
{
    HRESULT hr = S_OK;

    stde::com_ptr<IStream> spStream;
    ConfigurationPtr spProperties;
    std::vector<Mesh> meshes;

    ISNOTNULL(pRenderer, E_INVALIDARG);
    ISNOTNULL(spContentManager, E_INVALIDARG);

    CHECKHR(spContentManager->GetStream(contentId, &spStream));
    CHECKHR(Configuration::Load(spStream, &spProperties));

    {
        uint64 geometryId = spProperties->GetValue<uint64>("Mesh0_Geometry", 0);
        uint64 materialId = spProperties->GetValue<uint64>("Mesh0_Material", 0);

        Mesh mesh;
        mesh.LocalTransform.Matrix = Lucid3D::Matrix::Identity();
        CHECKHR(Geometry::Create(pRenderer, spContentManager, geometryId, true, mesh.spGeometry));
        CHECKHR(Material::Create(pRenderer, spContentManager, materialId, mesh.spMaterial));
        meshes.push_back(mesh);
    }

    spModel.attach(new Model(meshes));

EXIT
    if (FAILED(hr))
    {
        spModel.reset();
    }

    return hr;
}

GDK_IMETHODIMP Model::Process(_Inout_ RenderMap& renderMap, _In_ GDK::Transform& transform)
{
    for (size_t i = 0; i < _meshes.size(); i++)
    {
        Lucid3D::Matrix world(Lucid3D::Matrix::Multiply(_meshes[i].LocalTransform.Matrix, transform.Matrix));

        renderMap[_meshes[i].spMaterial->GetShader()].push_back(std::make_shared<RenderTask>(world, _meshes[i].spGeometry, _meshes[i].spMaterial, _meshes[i].spMaterial->GetShader()));
    }

    return S_OK;
}
