#include "RotatingObject.h"
#include "Quake2Game.h"
#include "QuakeProperties.h"
#include <GameObject.h>
#include "debug.h"

using namespace GDK;

namespace Quake2
{
    GameObjectCreateParameters RotatingObject::Create(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        UNREFERENCED_PARAMETER(gameWorld);

        GameObjectCreateParameters params;
        params.className = quakeProps->GetClassName();
        params.targetName = quakeProps->GetTargetName();
        params.position = quakeProps->GetOrigin();
        params.visuals = gameWorld->GetModel(quakeProps->GetModel());
        params.physicsType = PhysicsBodyType::Kinematic;
        params.controller.reset(GDKNEW RotatingObject(quakeProps->GetSpeed()));

        std::vector<Triangle> triangles;
        for (uint32_t i = 0; i < params.visuals.size(); ++i)
        {
            Collision::TriangleListFromGeometry(Content::LoadGeometryContent(params.visuals[i].geometry), Matrix::Identity(), 0, triangles);
        }
        params.collisionPrimitive = CollisionPrimitive::Create(TriangleMesh(SpacePartitionType::AabbTree, triangles));

        return params;
    }

    RotatingObject::RotatingObject(_In_ const float& speed)
    {
        _speed = Math::ToRadians(speed);
    }

    uint32_t RotatingObject::GetTypeID() const
    {
        return static_cast<uint32_t>(GameControllerType::Item);
    }

    void RotatingObject::OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject)
    {
        _gameObject = gameObject;
    }

    void RotatingObject::OnDestroy()
    {
    }

    void RotatingObject::OnUpdate()
    {
        if (_speed > 0)
        {
            auto gameObject = _gameObject.lock();
            if (gameObject)
            {
                float curRotation = gameObject->GetTransform().GetRotation();
                curRotation += gameObject->GetGameWorld()->GetTime().deltaTime * _speed;
                gameObject->GetTransform().SetRotation(curRotation);
            }
        }
    }

    void RotatingObject::OnActivate()
    {

    }

    void RotatingObject::AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        UNREFERENCED_PARAMETER(properties);
    }
}
