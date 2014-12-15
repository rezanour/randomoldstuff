#include "Door.h"
#include "Quake2Game.h"
#include "QuakeProperties.h"
#include <GameObject.h>

using namespace GDK;

namespace Quake2
{
    GameObjectCreateParameters Door::Create(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        UNREFERENCED_PARAMETER(gameWorld);

        GameObjectCreateParameters params;
        params.className = L"func_door";
        params.targetName = std::wstring(L"*") + std::to_wstring(quakeProps->GetModel());
        params.visuals = gameWorld->GetModel(quakeProps->GetModel());
        params.physicsType = PhysicsBodyType::Kinematic;

        params.position = quakeProps->GetOrigin();
        params.rotation = 0.0f;
        auto angle = quakeProps->GetAngle();

        Vector3 openPosition;
        if (fabsf(angle - Math::ToRadians(-1.0f)) < Math::Epsilon)
        {
            openPosition = Vector3(0, 100, 0);
        }
        else if (fabsf(angle - Math::ToRadians(-2.0f)) < Math::Epsilon)
        {
            openPosition = Vector3(0, -100, 0);
        }
        else
        {
            Vector3 dir = Vector3(0, 0, 1);
            dir = Matrix::TransformNormal(dir, Matrix::CreateRotationY(angle));
            openPosition = dir * 100;
        }

        auto door = std::shared_ptr<Door>(GDKNEW Door(openPosition));
        door->_angleToStore = angle;
        params.controller = door;

        std::vector<Triangle> triangles;
        for (uint32_t i = 0; i < params.visuals.size(); ++i)
        {
            Collision::TriangleListFromGeometry(Content::LoadGeometryContent(params.visuals[i].geometry), Matrix::Identity(), 0, triangles);
        }
        params.collisionPrimitive = CollisionPrimitive::Create(TriangleMesh(SpacePartitionType::AabbTree, triangles));

        // Add a trigger to open us
        std::map<std::wstring, std::wstring> triggerProps;
        triggerProps[L"classname"] = L"trigger_relay"; // switched from trigger_once to trigger_relay until more triggers are defined.
        triggerProps[L"target"] = params.targetName;
        triggerProps[L"model"] = std::wstring(L"*") + std::to_wstring(quakeProps->GetModel());
        triggerProps[L"dontsave"] = L"true";
        gameWorld->CreateObject(triggerProps);

        return params;
    }

    Door::Door(_In_ const Vector3& openPosition) :
        _openPosition(openPosition)
    {
    }

    uint32_t Door::GetTypeID() const
    {
        return static_cast<uint32_t>(GameControllerType::Item);
    }

    void Door::OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject)
    {
        _gameObject = gameObject;
        _closedPosition = gameObject.lock()->GetTransform().GetPosition();
        _targetPosition = _closedPosition;
    }

    void Door::OnDestroy()
    {
    }

    void Door::OnUpdate()
    {
        // TODO: move this
        static const float MoveSpeed = 50.0f;

        auto gameObject = _gameObject.lock();
        if (gameObject)
        {
            Vector3 toTarget = _targetPosition - gameObject->GetTransform().GetPosition();
            if (toTarget.LengthSquared() > Math::Epsilon)
            {
                float dist = toTarget.Length();
                Vector3 dir = Vector3::Normalize(toTarget);

                float movementDist = MoveSpeed * gameObject->GetGameWorld()->GetTime().deltaTime;
                if (movementDist > dist)
                {
                    movementDist = dist;
                }

                gameObject->GetTransform().SetPosition(gameObject->GetTransform().GetPosition() + dir * movementDist);
            }
        }

        // Always try to close the door, unless overridden by OnActivate()
        _targetPosition = _closedPosition;
    }

    void Door::OnActivate()
    {
        _targetPosition = _openPosition;
    }

    void Door::AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        if (_openPosition.y > 0)
        {
            properties[L"angle"] = std::to_wstring(-1.0f);
        }
        else if (_openPosition.y < 0)
        {
            properties[L"angle"] = std::to_wstring(-2.0f);
        }
        else
        {
            properties[L"angle"] = StringUtils::ToString<float>(Math::ToDegrees(_angleToStore));
        }
    }
}
