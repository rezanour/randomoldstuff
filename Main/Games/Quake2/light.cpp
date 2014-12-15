#include "Light.h"
#include "Quake2Game.h"
#include "QuakeProperties.h"
#include <GameObject.h>
#include "debug.h"

using namespace GDK;

namespace Quake2
{
    GameObjectCreateParameters Light::Create(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        UNREFERENCED_PARAMETER(gameWorld);

        GameObjectCreateParameters params;
        params.className = quakeProps->GetClassName();
        params.targetName = quakeProps->GetTargetName();
        params.position = quakeProps->GetOrigin();
        params.controller.reset(GDKNEW Light());

        return params;
    }

    Light::Light()
    {

    }

    uint32_t Light::GetTypeID() const
    {
        return static_cast<uint32_t>(GameControllerType::Item);
    }

    void Light::OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject)
    {
        _gameObject = gameObject;
    }

    void Light::OnDestroy()
    {
    }

    void Light::OnUpdate()
    {
    }

    void Light::OnActivate()
    {

    }

    void Light::AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        UNREFERENCED_PARAMETER(properties);
    }
}
