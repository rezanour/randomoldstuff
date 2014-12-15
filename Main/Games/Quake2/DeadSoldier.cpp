#include "DeadSoldier.h"
#include "Quake2Game.h"
#include "QuakeProperties.h"
#include <GameObject.h>

using namespace GDK;

namespace Quake2
{
    GameObjectCreateParameters DeadSoldier::Create(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        UNREFERENCED_PARAMETER(gameWorld);

        GameObjectCreateParameters params;
        params.className = quakeProps->GetClassName();
        params.visuals.push_back(VisualInfo(Matrix::Identity(), L"models\\deadbods\\dude\\tris.md2.geometry", L"models\\deadbods\\dude\\dead1.pcx.texture"));
        params.physicsType = PhysicsBodyType::Sensor;
        params.collisionPrimitive = Collision::AlignedCapsuleFromGeometry(Content::LoadGeometryContent(L"models\\deadbods\\dude\\tris.md2.geometry"), 0, 0.0f);
        params.controller.reset(GDKNEW DeadSoldier());

        params.position = quakeProps->GetOrigin();
        params.rotation = quakeProps->GetAngle();

        return params;
    }

    DeadSoldier::DeadSoldier()
    {
    }

    uint32_t DeadSoldier::GetTypeID() const
    {
        return static_cast<uint32_t>(GameControllerType::Item);
    }

    void DeadSoldier::OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject)
    {
        UNREFERENCED_PARAMETER(gameObject);
    }

    void DeadSoldier::OnDestroy()
    {
    }

    void DeadSoldier::OnUpdate()
    {

    }

    void DeadSoldier::OnActivate()
    {

    }

    void DeadSoldier::AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        UNREFERENCED_PARAMETER(properties);
    }
}
