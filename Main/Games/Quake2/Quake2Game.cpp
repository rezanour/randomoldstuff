#include "Quake2Game.h"
#include "Menu.h"
#include "Hud.h"
#include "Player.h"
#include "Monster.h"
#include "Door.h"
#include "SimpleAction.h"
#include "Trigger.h"
#include "light.h"
#include "RotatingObject.h"
#include "DeadSoldier.h"
#include "Barrel.h"
#include "QuakeProperties.h"
#include <GameWorld.h>
#include <SpacePartition.h>
#include <Transform.h>

using namespace GDK;

namespace Quake2
{
    static GameObjectCreateParameters CreateGameObject(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::map<std::wstring, std::wstring>& properties);

    std::shared_ptr<GDK::ContentCache> Quake2Game::_content;
    std::shared_ptr<GDK::ContentCache> Quake2Game::GetContentCache()
    {
        return _content;
    }

    void Initialize()
    {
        static bool initialized = false;

        if (!initialized)
        {
            Quake2Game::InitializeGameInfo();
            initialized = true;
        }
    }

    void Quake2Game::InitializeGameInfo()
    {
        GameObject::RegisterFactory(CreateGameObject);

        // Initialize game
        GameInfo gameInfo;
        gameInfo.name = L"Quake 2 GDK Edition";
        gameInfo.firstWorld = L"";
        gameInfo.onCreate = Quake2Game::OnCreate;

        Game::RegisterGame(gameInfo);
    }

    std::shared_ptr<IGame> Quake2Game::OnCreate(_In_ const DeviceContext& deviceContext)
    {
        return std::shared_ptr<IGame>(GDKNEW Quake2Game(deviceContext));
    }

    Quake2Game::Quake2Game(_In_ const DeviceContext& deviceContext) :
        _deviceContext(deviceContext)
    {
        _testIndex = 0;

        // Create static content cache
        _content = GDK::ContentCache::Create(deviceContext);

        // Bind our keys
        GDK::Input::BindButton(QuakeKey::MoveLeft, GDK::Button::A);
        GDK::Input::BindButton(QuakeKey::MoveRight, GDK::Button::D);
        GDK::Input::BindButton(QuakeKey::MoveForward, GDK::Button::W);
        GDK::Input::BindButton(QuakeKey::MoveBackward, GDK::Button::S);
        GDK::Input::BindButton(QuakeKey::TurnRight, GDK::Button::Right);
        GDK::Input::BindButton(QuakeKey::TurnLeft, GDK::Button::Left);
        GDK::Input::BindButton(QuakeKey::Fire, GDK::Button::MouseLeft);
        GDK::Input::BindButton(QuakeKey::Jump, GDK::Button::Space);
        GDK::Input::BindButton(QuakeKey::Menu, GDK::Button::Escape);
        GDK::Input::BindButton(QuakeKey::QuickQuit, GDK::Button::Tab);
        GDK::Input::BindButton(QuakeKey::MenuUp, GDK::Button::Up);
        GDK::Input::BindButton(QuakeKey::MenuDown, GDK::Button::Down);
        GDK::Input::BindButton(QuakeKey::MenuSelect, GDK::Button::Enter);
        GDK::Input::BindButton(QuakeKey::Yes, GDK::Button::Y);
        GDK::Input::BindButton(QuakeKey::No, GDK::Button::N);
        GDK::Input::BindButton(QuakeKey::SelectBlaster, GDK::Button::D1);
        GDK::Input::BindButton(QuakeKey::SelectShotgun,GDK::Button::D2);
        GDK::Input::BindButton(QuakeKey::SelectSuperShotgun,GDK::Button::D3);
        GDK::Input::BindButton(QuakeKey::SelectMachinegun,GDK::Button::D4);
        GDK::Input::BindButton(QuakeKey::SelectChaingun,GDK::Button::D5);
        GDK::Input::BindButton(QuakeKey::SelectGrenadeLauncher,GDK::Button::D6);
        GDK::Input::BindButton(QuakeKey::SelectRocketLauncher,GDK::Button::D7);
        GDK::Input::BindButton(QuakeKey::SelectHyperBlaster,GDK::Button::D8);
        GDK::Input::BindButton(QuakeKey::SelectRailgun,GDK::Button::D9);
        GDK::Input::BindButton(QuakeKey::SelectBFG,GDK::Button::D0);
        GDK::Input::BindButton(QuakeKey::TestIt,GDK::Button::T);

        _mainMenu = MainMenu::Create(_deviceContext);
        _hud = Hud::Create(_deviceContext);
    }

    _Use_decl_annotations_
    void Quake2Game::OnWorldChanged(const std::shared_ptr<IGameWorld>& gameWorld)
    {
        std::vector<std::shared_ptr<IGameObject>> players;

        gameWorld->FindObjectsByClassName(L"player", players);
        CHECK_FALSE(players.empty());
        _player = players[0];
        static_cast<Player*>(_player->GetController().get())->SetHud(_hud);
    }

    _Use_decl_annotations_
    UpdateResult Quake2Game::OnUpdateBegin(const GameTime& gameTime)
    {
        // Before the game ticks, let's check our essentials
        if (GDK::Input::WasButtonPressed(QuakeKey::QuickQuit))
        {
            return UpdateResult::Exit;
        }

        static_cast<Player*>(_player->GetController().get())->EnableInput(!_mainMenu->IsVisible());

        if (_mainMenu->IsVisible())
        {
            // Update menu overlay
            if (!_mainMenu->Update(gameTime))
            {
                return UpdateResult::Exit;
            }
        }
        else
        {
            _hud->Update(gameTime);
        }

        if (GDK::Input::WasButtonPressed(QuakeKey::Menu))
        {
            _mainMenu->Show(!_mainMenu->IsVisible());
        }

        if (GDK::Input::WasButtonPressed(QuakeKey::TestIt))
        {
            std::vector<std::shared_ptr<GDK::IGameObject>> objects;
            _player->GetGameWorld()->FindObjectsByClassName(L"target_speaker", objects);
            if (objects.size())
            {
                objects[_testIndex]->GetController()->OnActivate();
                _testIndex++;
                if (_testIndex >= objects.size())
                {
                    _testIndex = 0;
                }
            }
        }

        return UpdateResult::Continue;
    }

    _Use_decl_annotations_
    UpdateResult Quake2Game::OnUpdateEnd(const GameTime& gameTime)
    {
        UNREFERENCED_PARAMETER(gameTime);
        auto player = static_cast<Player*>(_player->GetController().get());
        Vector3 position = _player->GetTransform().GetPosition() + Vector3::Up() * (player->GetHeight() * 0.5f);
        auto world = _player->GetTransform().GetWorld();
        world.SetTranslation(position);
        _hud->SetOwnerWorldMatrix(Matrix::CreateRotationX(player->GetAim()) * world);
        return UpdateResult::Continue;
    }

    _Use_decl_annotations_
    void Quake2Game::OnDrawBegin(Matrix* view, Matrix* projection)
    {
        static const float nearDist = 1.0f;
        static const float farDist = 10000.0f;
        static const float fov = Math::PiOver4;
        static const float aspectRatio = 16.0f / 9.0f;
        static const float yFovOver2 = atan2(4.5f, tan(fov/2)); // based on 16/9

        Player* player = static_cast<Player*>(_player->GetController().get());

        Matrix world = _player->GetTransform().GetWorld();
        float aim = player->GetAim();
        Matrix aimRotation = Matrix::CreateFromAxisAngle(world.GetRight(), aim);

        Vector3 forward = Vector3::Normalize(world.GetForward() * aimRotation);
        Vector3 up = Vector3::Cross(forward, world.GetRight());

        Vector3 position = _player->GetTransform().GetPosition() + Vector3::Up() * (player->GetHeight() * 0.5f);
        *view = Matrix::CreateLookAt(position, position + forward, up);
        *projection = Matrix::CreatePerspectiveFov(fov, aspectRatio, nearDist, farDist);
    }

    _Use_decl_annotations_
    void Quake2Game::OnDrawEnd(const Matrix& view, const Matrix& projection)
    {
        UNREFERENCED_PARAMETER(view);
        UNREFERENCED_PARAMETER(projection);

        _hud->Show(true);

        _deviceContext.graphicsDevice->EnableZBuffer(false);

        if (_hud->IsVisible())
        {
            // Render hud overlay
            _hud->Render();
        }

        if (_mainMenu->IsVisible())
        {
            // Render menu overlay
            _mainMenu->Render();
        }

        _deviceContext.graphicsDevice->EnableZBuffer(true);
    }

    //
    // Game object factory
    //
    static GameObjectCreateParameters CreateStaticWorld(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::shared_ptr<QuakeProperties>& quakeProps)
    {
        // the static part of the world is really just some visuals & a triangle mesh collision object
        GameObjectCreateParameters params;
        params.className = L"world";
        params.visuals = gameWorld->GetModel(0);
        params.physicsType = PhysicsBodyType::Kinematic;

        std::vector<Triangle> triangles;
        for (uint32_t i = 0; i < params.visuals.size(); ++i)
        {
            Collision::TriangleListFromGeometry(Content::LoadGeometryContent(params.visuals[i].geometry), Matrix::Identity(), 0, triangles);
        }
        params.collisionPrimitive = CollisionPrimitive::Create(TriangleMesh(SpacePartitionType::AabbTree, triangles));
        params.position = quakeProps->GetOrigin();
        params.rotation = quakeProps->GetAngle();

        return params;
    }

    GameObjectCreateParameters CreateGameObject(_In_ const std::shared_ptr<IGameWorld>& gameWorld, _In_ const std::map<std::wstring, std::wstring>& properties)
    {
        auto classNameEntry = properties.find(L"classname");
        if (classNameEntry != properties.end())
        {
            auto& className = classNameEntry->second;

            std::shared_ptr<QuakeProperties> quakeProps = QuakeProperties::Create(properties);

            if (className == L"worldspawn")
            {
                return CreateStaticWorld(gameWorld, quakeProps);
            }
            else if (className == L"target_speaker" || className == L"target_secret" || className == L"target_help")
            {
                return SimpleAction::Create(gameWorld, quakeProps);
            }
            else if (className == L"func_door")
            {
                return Door::Create(gameWorld, quakeProps);
            }
            else if (className == L"trigger_once")
            {
                return Trigger::Create(gameWorld, quakeProps);
            }
            else if (className == L"trigger_relay")
            {
                return Trigger::Create(gameWorld, quakeProps);
            }
            else if (className == L"func_timer")
            {
                return Trigger::Create(gameWorld, quakeProps);
            }
            else if (className == L"info_player_start" && properties.find(L"targetname") == properties.end())
            {
                return Player::Create(gameWorld, quakeProps);
            }
            else if (className.find(L"monster_") != className.npos)
            {
                return Monster::Create(gameWorld, quakeProps);
            }
            else if (className.find(L"item_") != className.npos)
            {
                return Item::Create(gameWorld, quakeProps);
            }
            else if (className.find(L"weapon_") != className.npos)
            {
                return Item::Create(gameWorld, quakeProps);
            }
            else if (className.find(L"ammo_") != className.npos)
            {
                return Item::Create(gameWorld, quakeProps);
            }
            else if (className.find(L"light") != className.npos)
            {
                return Light::Create(gameWorld, quakeProps);
            }
            else if (className.find(L"func_rotating") != className.npos)
            {
                return RotatingObject::Create(gameWorld, quakeProps);
            }
            else if (className.find(L"misc_deadsoldier") != className.npos)
            {
                return DeadSoldier::Create(gameWorld, quakeProps);
            }
            else if (className.find(L"misc_explobox") != className.npos)
            {
                return Barrel::Create(gameWorld, quakeProps);
            }
            else
            {
                // Create a default object for unknown objects
                return Item::CreatePlaceholderItem(gameWorld, quakeProps);
            }
        }

        // no class name or not handled elsewhere? Error
        throw std::invalid_argument("classname");
    }
}
