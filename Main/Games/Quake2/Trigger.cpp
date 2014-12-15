#include "Trigger.h"
#include "Quake2Game.h"
#include <GameObject.h>
#include <GameWorld.h>
#include "debug.h"

namespace Quake2
{
    GameObjectCreateParameters Trigger::Create(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        GameObjectCreateParameters params;
        params.className = quakeProps->GetClassName();
        int model = quakeProps->GetModel();
        if (model != -1)
        {
            auto& visuals = gameWorld->GetModel(model);
            params.physicsType = PhysicsBodyType::Sensor;

            std::vector<Triangle> triangles;
            for (auto& visual : visuals)
            {
                Collision::TriangleListFromGeometry(Content::LoadGeometryContent(visual.geometry), Matrix::Identity(), 0, triangles);
            }

            params.collisionPrimitive = Collision::AlignedCapsuleFromTriangleList(triangles, 0.0f);
        }

        params.targetName = quakeProps->GetTargetName();

        if (params.className == L"trigger_once")
        {
            params.controller.reset(GDKNEW Trigger(Type::Once, quakeProps->GetTarget(), quakeProps->GetDelay()));
        }
        else if (params.className == L"trigger_always")
        {
            params.controller.reset(GDKNEW Trigger(Type::Always, quakeProps->GetTarget(), quakeProps->GetDelay()));
        }
        else if (params.className == L"trigger_multiple")
        {
            params.controller.reset(GDKNEW Trigger(Type::Multiple, quakeProps->GetTarget(), quakeProps->GetDelay()));
        }
        else if (params.className == L"func_timer")
        {
            params.controller.reset(GDKNEW Trigger(Type::Timer, quakeProps->GetTarget(), quakeProps->GetWait()));
        }
        else
        {
            params.controller.reset(GDKNEW Trigger(Type::Relay, quakeProps->GetTarget(), quakeProps->GetDelay()));
        }
        params.position = quakeProps->GetOrigin();
        params.rotation = quakeProps->GetAngle();

        return params;
    }

    Trigger::Trigger(_In_ Type type, _In_ const std::wstring& target, _In_ const float delay) :
        _type(type), _target(target),_delay(delay), _elapsedTime(0.0f), _fired(false)
    {
        _delay *= 60; // delay is in minutes?
        _expired = false;
    }

    uint32_t Trigger::GetTypeID() const
    {
        return static_cast<uint32_t>(GameControllerType::Trigger);
    }

    void Trigger::OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject)
    {
        _gameObject = gameObject;
    }

    void Trigger::OnDestroy()
    {
    }

    void Trigger::OnActivate()
    {
        // Trigger has been activated, signal the activated state and handle processing in the trigger's
        // OnUpdate() loop.
        if (!_fired)
        {
            //DebugOut("TRIGGER FIRED IN ACTIVATE - TRIGGER (%d): DELAY = %f", _type ,_delay);
            _fired = true;
        }
    }

    void Trigger::OnUpdate()
    {
        if (_expired)
        {
            return;
        }

        auto gameObject = _gameObject.lock();
        if (gameObject)
        {
            //  Get all targets associated with this trigger
            if (_targets.size() == 0)
            {
                gameObject->GetGameWorld()->FindObjectsByTargetName(_target, _targets);
            }

            // If the trigger is in the fired state, perform activations
            if (_fired && !_expired)
            {
                _elapsedTime += gameObject->GetGameWorld()->GetTime().deltaTime;
                if (_elapsedTime >= _delay)
                {
                    _elapsedTime = 0.0f;

                    for (auto& target : _targets)
                    {
                        DebugOut("ACTIVATING - %S(%S)",target->GetClassName().c_str(), target->GetTargetName().c_str());
                        target->GetController()->OnActivate();
                    }
                    _fired = false;

                    if (_type == Type::Once)
                    {
                        DebugOut("%S(%S)(EXPIRED): DELAY = %f", gameObject->GetClassName().c_str(), gameObject->GetTargetName().c_str() ,_delay);
                        _expired = true;
                    }
                }
            }

            // Check for player intersections
            auto& contacts = gameObject->GetContacts();
            for (auto& contact : contacts)
            {
                if (contact.other != nullptr && static_cast<GameControllerType>(contact.other->GetController()->GetTypeID()) == GameControllerType::Player)
                {
                    // Contact with the player has been detected, activate targets?
                    OnActivate();
                    break;
                }
            }
        }
    }

    void Trigger::AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        UNREFERENCED_PARAMETER(properties);
    }
}
