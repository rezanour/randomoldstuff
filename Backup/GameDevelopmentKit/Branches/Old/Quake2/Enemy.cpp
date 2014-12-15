#include "Precomp.h"
#include "Enemy.h"

namespace Quake2
{

using namespace GDK;
using Microsoft::WRL::ComPtr;

ComPtr<GameObject> GDKAPI Enemy::Create(_In_ const std::wstring& type, _In_ GDK::Graphics::IGraphicsDevice* graphics, _In_ GDK::Content::IContent* content)
{
    ComPtr<Enemy> enemy = Make<Enemy>(type);

    ComPtr<Content::IGeometryResource> geometryResource;
    CHECKHR(content->GetGeometryResource(L"enemies\\soldier.object\\mesh.geometry", geometryResource.GetAddressOf()));

    ComPtr<Content::ITextureResource> textureResource;
    CHECKHR(content->GetTextureResource(L"enemies\\soldier.object\\primary.texture", textureResource.GetAddressOf()));

    ComPtr<Graphics::IRuntimeGeometry> runtimeGeometry;
    CHECKHR(graphics->CreateRuntimeGeometry(geometryResource.Get(), runtimeGeometry.GetAddressOf()));

    ComPtr<Graphics::IRuntimeTexture> runtimeTexture;
    CHECKHR(graphics->CreateRuntimeTexture(textureResource.Get(), runtimeTexture.GetAddressOf()));

    enemy->SetGraphicsAssets(runtimeGeometry.Get(), runtimeTexture.Get());

    return enemy;
}

Enemy::Enemy(_In_ const std::wstring& type) :
    GameObject(type),
    _health(100),
    _armor(100)
{
}

Enemy::~Enemy()
{
}

void GDKAPI Enemy::ConfigureProperties(_Inout_ GameObject::PropertyMap& properties)
{
    properties[L"Health"] = Make<IntegerProperty>(L"Health", &_health);
    properties[L"Armor"] = Make<IntegerProperty>(L"Armor", &_armor);
}

} // Quake2
