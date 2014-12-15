#include "SimpleAction.h"
#include "Quake2Game.h"
#include "QuakeProperties.h"
#include <GameObject.h>
#include "debug.h"

using namespace GDK;

namespace Quake2
{
    GameObjectCreateParameters SimpleAction::Create(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        UNREFERENCED_PARAMETER(gameWorld);

        GameObjectCreateParameters params;
        params.className = quakeProps->GetClassName();
        params.targetName = quakeProps->GetTargetName();
        params.position = quakeProps->GetOrigin();
        if (params.className == L"target_speaker")
        {
            params.controller.reset(GDKNEW SimpleAction(SimpleAction::Type::PlaySound, quakeProps->GetSoundName()));
        }
        else if (params.className == L"target_secret" || params.className == L"target_help")
        {
            params.controller.reset(GDKNEW SimpleAction(SimpleAction::Type::PrintText, quakeProps->GetMessage()));
        }

        return params;
    }

    SimpleAction::SimpleAction(_In_ Type type, _In_ std::wstring data) :
        _type(type), _data(data), _actionComplete(false)
    {

    }

    uint32_t SimpleAction::GetTypeID() const
    {
        return static_cast<uint32_t>(GameControllerType::Item);
    }

    void SimpleAction::OnCreate(_In_ const std::weak_ptr<IGameObject>& gameObject)
    {
        _gameObject = gameObject;
    }

    void SimpleAction::OnDestroy()
    {
    }

    void SimpleAction::OnUpdate()
    {
    }

    void SimpleAction::OnActivate()
    {
        auto gameObject = _gameObject.lock();
        auto gameWorld = gameObject->GetGameWorld();

        if (!_actionComplete)
        {
            //_actionComplete = true;

            switch(_type)
            {
            case SimpleAction::Type::PlaySound:
                // Play sound
                DebugOut("PLAY SOUND: %S",_data.c_str());
                gameWorld->GetDeviceContext().audioDevice->PlayClip(Quake2Game::GetContentCache()->GetAudioClip(_data));
                break;
            case SimpleAction::Type::PrintText:
                // Print text to window
                // _data is the text to output
                DebugOut("TEXT MESSAGE: %S",_data.c_str());
                break;
            default:
                break;
            }
        }
    }

    void SimpleAction::AppendProperties(_Inout_ std::map<std::wstring, std::wstring>& properties) const
    {
        UNREFERENCED_PARAMETER(properties);
    }

    SimpleAction::Type SimpleAction::GetItemType() const
    {
        return _type;
    }

    uint16_t SimpleAction::GetValue() const
    {
        return _value;
    }
}
