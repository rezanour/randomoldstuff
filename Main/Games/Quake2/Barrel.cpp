#include "Barrel.h"
#include "Quake2Game.h"
#include "QuakeProperties.h"
#include <GameObject.h>

using namespace GDK;

namespace Quake2
{
    GameObjectCreateParameters Barrel::Create(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        UNREFERENCED_PARAMETER(gameWorld);

        GameObjectCreateParameters params;
        params.className = quakeProps->GetClassName();
        params.visuals.push_back(VisualInfo(Matrix::Identity(), L"models\\objects\\barrels\\tris.md2.geometry", L"models\\objects\\barrels\\skin.pcx.texture"));
        params.physicsType = PhysicsBodyType::Sensor;
        params.collisionPrimitive = Collision::AlignedCapsuleFromGeometry(Content::LoadGeometryContent(L"models\\objects\\barrels\\tris.md2.geometry"), 0, 0.0f);
        params.controller.reset(GDKNEW Barrel());

        params.position = quakeProps->GetOrigin();
        params.rotation = quakeProps->GetAngle();

        return params;
    }

    Barrel::Barrel()
    {
    }

    uint32_t Barrel::GetTypeID() const
    {
        return static_cast<uint32_t>(GameControllerType::Item);
    }

    void Barrel::OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject)
    {
        UNREFERENCED_PARAMETER(gameObject);
    }

    void Barrel::OnDestroy()
    {
    }

    void Barrel::OnUpdate()
    {

    }

    void Barrel::OnActivate()
    {

    }

    void Barrel::AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        UNREFERENCED_PARAMETER(properties);
    }
}
