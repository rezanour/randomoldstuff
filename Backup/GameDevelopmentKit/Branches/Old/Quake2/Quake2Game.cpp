#include "Precomp.h"
#include "Quake2Game.h"
#include "GameWorld.h"

using namespace GDK;
using Microsoft::WRL::ComPtr;

#define ENSURE_EDITING { if (!_editing) throw GDK::Exception(E_ACCESSDENIED, L"Game not in editing mode!", __FILEW__, __LINE__); }

namespace Quake2 {

const wchar_t* const Quake2Game::DisplayName = L"Quake2 GDK Edition";

HRESULT GDKAPI Quake2GameFactory::CreateGame(_In_ const GameCreationParameters& parameters, _In_opt_z_ const wchar_t* initialWorld, _COM_Outptr_ IGame** game)
{
    MODULE_GUARD_BEGIN

    Quake2Game::Create(parameters, initialWorld).CopyTo(game);

    MODULE_GUARD_END
}

HRESULT GDKAPI Quake2GameFactory::CreateGameEdit(_In_ const GameCreationParameters& parameters, _COM_Outptr_ IGameEdit** game)
{
    MODULE_GUARD_BEGIN

    Quake2Game::CreateEdit(parameters).CopyTo(game);

    MODULE_GUARD_END
}

ComPtr<Quake2Game> GDKAPI Quake2Game::Create(_In_ const GameCreationParameters& parameters, _In_opt_z_ const wchar_t* initialWorld)
{
    CHECK_NOT_NULL(parameters.host, E_INVALIDARG);
    CHECK_NOT_NULL(parameters.content, E_INVALIDARG);
    CHECK_NOT_NULL(parameters.graphicsDevice, E_INVALIDARG);

    ComPtr<Quake2Game> game = Make<Quake2Game>(parameters, false);

    game->_world = GameWorld::Create(game.Get(), initialWorld);

    return game;
}

ComPtr<Quake2Game> GDKAPI Quake2Game::CreateEdit(_In_ const GameCreationParameters& parameters)
{
    CHECK_NOT_NULL(parameters.host, E_INVALIDARG);
    CHECK_NOT_NULL(parameters.content, E_INVALIDARG);
    CHECK_NOT_NULL(parameters.graphicsDevice, E_INVALIDARG);

    ComPtr<Quake2Game> game = Make<Quake2Game>(parameters, true);

    game->_world = GameWorld::Create(game.Get(), L"untitled world");

#ifdef WIN32
    ComPtr<GDK::IStream> stream;
    CHECKHR(CreateFileStream(L"temp.world", false, &stream));
    CHECKHR(game->_world->Save(stream.Get()));

    stream.Reset();

    CHECKHR(CreateFileStream(L"temp.world", true, stream.ReleaseAndGetAddressOf()));
    ComPtr<IWorldEdit> world;
    CHECKHR(game->LoadWorld(stream.Get(), &world));
#endif

    return game;
}

Quake2Game::Quake2Game(_In_ const GDK::GameCreationParameters& parameters, _In_ bool editing) :
    _host(parameters.host),
    _graphicsDevice(parameters.graphicsDevice),
    _content(parameters.content),
    _editing(editing)
{
}

Quake2Game::~Quake2Game()
{
}

//
// IGame
//
const wchar_t* GDKAPI Quake2Game::GetName() const
{
    return DisplayName;
}

HRESULT GDKAPI Quake2Game::Update(_In_ double elapsedSeconds)
{
    if (_world != nullptr)
    {
        _world->Update(elapsedSeconds);
    }

    return S_OK;
}

HRESULT GDKAPI Quake2Game::Draw(_In_ const Matrix& view, _In_ const Matrix& projection)
{
    _graphicsDevice->SetViewProjection(view, projection);
    if (_world != nullptr)
    {
        _world->Draw();
    }
    return S_OK;
}

//
// IGameEdit
//
HRESULT GDKAPI Quake2Game::CreateWorld(_In_opt_z_ const wchar_t* name, _COM_Outptr_ GDK::IWorldEdit** blankWorld)
{
    MODULE_GUARD_BEGIN

    CHECK_NOT_NULL(blankWorld, E_POINTER);
    *blankWorld = nullptr;

    ENSURE_EDITING;

    ComPtr<GameWorld> world = GameWorld::Create(this, name);

    CHECKHR(world.CopyTo(blankWorld));

    MODULE_GUARD_END
}

HRESULT GDKAPI Quake2Game::LoadWorld(_In_ GDK::IStream* stream, _COM_Outptr_ GDK::IWorldEdit** world)
{
    MODULE_GUARD_BEGIN

    ENSURE_EDITING;

    if (!world)
    {
        return E_POINTER;
    }
    *world = nullptr;

    auto theWorld = GameWorld::Create(this, stream);
    CHECKHR(theWorld.CopyTo(world));

    MODULE_GUARD_END
}

HRESULT GDKAPI Quake2Game::SetWorld(_In_ GDK::IWorldEdit* world)
{
    ENSURE_EDITING;

    UNREFERENCED_PARAMETER(world);

    return E_NOTIMPL;
}

HRESULT GDKAPI Quake2Game::GetRequiredResources(_COM_Outptr_ GDK::IStringList** requiredResources) const
{
    static const wchar_t* resourceNames[] = 
    {
        // Soldier
        L"enemies\\soldier.object\\mesh.geometry",
        L"enemies\\soldier.object\\primary.texture",
        L"enemies\\soldier.object\\secondary.texture",

        // Brain enemy
        L"enemies\\brain.object\\mesh.geometry",
        L"enemies\\brain.object\\primary.texture",
        L"enemies\\brain.object\\secondary.texture",

        // Medkit/Health
        L"pickups\\medkit_medium.object\\mesh.geometry",
        L"pickups\\medkit_medium.object\\primary.texture",
        L"pickups\\medkit_medium.object\\secondary.texture",
        L"pickups\\medkit_large.object\\mesh.geometry",
        L"pickups\\medkit_large.object\\primary.texture",
        L"pickups\\medkit_large.object\\secondary.texture",
        L"pickups\\medkit_stimpack.object\\mesh.geometry",
        L"pickups\\medkit_stimpack.object\\primary.texture",
        L"pickups\\medkit_stimpack.object\\secondary.texture",

        // Weapons (equipped)
        L"weapons\\bfg.object\\mesh.geometry",
        L"weapons\\bfg.object\\primary.texture",

        // Level 1
        L"statics\\level1background.object\\mesh.geometry",
        L"statics\\level1background.object\\primary.texture",
        L"statics\\level1background.object\\secondary.texture",
        L"levels\\level1.world",
    };

    return GDK::CreateStringList(resourceNames, _countof(resourceNames), requiredResources);
}

HRESULT GDKAPI Quake2Game::SetSelectionColor(_In_ const GDK::Vector4& color)
{
    UNREFERENCED_PARAMETER(color);
    return S_OK;
}

HRESULT GDKAPI Quake2Game::SetEditWidget(_In_ GDK::Graphics::IRuntimeGeometry* widgetGeometry)
{
    UNREFERENCED_PARAMETER(widgetGeometry);
    return S_OK;
}

} // Quake2
