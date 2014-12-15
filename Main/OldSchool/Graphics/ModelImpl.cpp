#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"
#include "ModelImpl.h"

uint32_t Model::s_nextId = 0;

_Use_decl_annotations_
std::shared_ptr<Model> Model::Create(const std::shared_ptr<IReadonlyObject>& owner)
{
    std::shared_ptr<Model> model(new Model);
    model->SetOwner(owner);
    return model;
}

Model::Model()
    : _token(0), _isTransparent(false), _id(s_nextId++)
{
}

_Use_decl_annotations_
void Model::SetGeometry(const std::shared_ptr<IGeometry>& geometry)
{
    _geometry = geometry;
}

_Use_decl_annotations_
void Model::SetTexture(TextureUsage usage, const std::shared_ptr<ITexture>& texture)
{
    uint32_t index = static_cast<uint32_t>(usage);
    assert(index < static_cast<uint32_t>(TextureUsage::Max));

    _textures[index] = texture;
}

_Use_decl_annotations_
void Model::SetOwner(const std::shared_ptr<IReadonlyObject>& owner)
{
    auto prev = _owner.lock();
    if (prev != nullptr)
    {
        prev->UnregisterFromAABBChangeNotifications(_token);
        _token = 0;
    }

    _owner = owner;

    auto next = _owner.lock();
    if (next != nullptr)
    {
        _token = next->RegisterForAABBChangeNotifications(std::bind(&Model::OnAABBChanged, this));
    }
}

_Use_decl_annotations_
void Model::EnableTransparency(bool enable)
{
    if (enable != _isTransparent)
    {
        _isTransparent = enable;
        auto scene = _scene.lock();
        if (scene != nullptr)
        {
            scene->ModelChangedTransparency(shared_from_this(), _isTransparent);
        }
    }

}

uint32_t Model::GetId() const
{
    return _id;
}

_Use_decl_annotations_
void Model::SetScene(const std::shared_ptr<GraphicsScene>& scene)
{
    auto oldScene = _scene.lock();
    _scene = scene;

    if (oldScene != nullptr)
    {
        oldScene->RemoveModel(shared_from_this());
    }

}

const std::shared_ptr<IGeometry>& Model::GetGeometry() const
{
    return _geometry;
}

const std::shared_ptr<ITexture>& Model::GetTexture(_In_ TextureUsage usage) const
{
    uint32_t index = static_cast<uint32_t>(usage);
    assert(index < static_cast<uint32_t>(TextureUsage::Max));

    return _textures[index];
}

std::shared_ptr<IReadonlyObject> Model::GetOwner() const
{
    return _owner.lock();
}

bool Model::IsTransparent() const
{
    return _isTransparent;
}

void Model::OnAABBChanged()
{
}
