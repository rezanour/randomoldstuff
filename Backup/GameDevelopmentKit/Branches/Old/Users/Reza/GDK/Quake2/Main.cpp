#include <Windows.h>
#include "Quake2Game.h"
#include "Enemies\Soldier.h"
#include <GameWorld.h>
#include <exception>
#include <stdexcept>

using namespace GDK;

// TODO: Move this into engine initialization, once we have a dedicated spot for that
// TODO: Why doesn't std::set_unexpected work?? We can't use SetUhandledExceptionFilter on non-MS. Maybe other plats have similar functions?
LONG WINAPI GDKHandleException(_In_ struct _EXCEPTION_POINTERS* exceptionPointers)
{
    // std::exceptions (and anything derived from them) have the following attributes
    if (exceptionPointers->ExceptionRecord->ExceptionCode == 0xe06d7363 && 
        exceptionPointers->ExceptionRecord->NumberParameters == 4 &&
        exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 0x19930520)
    {
        wchar_t buffer[2000] = {0};

        std::exception* ex = reinterpret_cast<std::exception*>(exceptionPointers->ExceptionRecord->ExceptionInformation[1]);
        GDKException* gdk = dynamic_cast<GDKException*>(ex);

        if (gdk)
        {
            swprintf_s(buffer, L"%S failed with error: 0x%08x\n", gdk->what(), gdk->GetErrorCode());
        }
        else
        {
            swprintf_s(buffer, L"%S\n", ex->what());
        }

        OutputDebugString(buffer);
        MessageBox(nullptr, buffer, L"GDK Error", MB_ICONERROR | MB_OK);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

int CALLBACK WinMain(
    _In_ HINSTANCE instance,
    _In_opt_ HINSTANCE prevInstance,
    _In_ LPSTR commandLine,
    _In_ int showCommand)
{
    UNREFERENCED_PARAMETER(instance);
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(commandLine);
    UNREFERENCED_PARAMETER(showCommand);

    SetUnhandledExceptionFilter(GDKHandleException);

    // One time startup registration of type system
    GameObject::RegisterFactory(L"enemies\\soldier.object", Soldier::Create);

    GameCreationParameters parameters;

    //GraphicsDeviceCreationParameters gdParameters;
    //parameters.GraphicsDevice = GraphicsDevice::CreateDevice(GraphicsDevice::Type::DirectX11, gdParameters);

    auto content = Content::CreateFileBased(L"C:\\GameDevelopmentKit\\Quake2\\Content");
    parameters.Content = content;

    auto texture = content->LoadTexture(L"enemies\\soldier.object\\primary.texture");

    auto game = std::make_shared<Quake2Game>(parameters);

    OutputDebugString(L"Game: ");
    OutputDebugString(game->GetName().c_str());
    OutputDebugString(L"\n");

    auto soldier1 = GameObject::Create(L"enemies\\soldier.object");
    auto soldier2 = GameObject::Create(L"enemies\\soldier.object");

    auto world = GameWorld::CreateNew(L"E1:M1");

    world->AddObject(soldier1);
    world->AddObject(soldier2);

    world->Update(0.016);

    auto& objects = world->GetObjects();
    for (auto obj : objects)
    {
        obj->SetPosition(obj->GetPosition() + GDK::Vector3(10, 0, 0));
    }

    world->Draw();

    for (size_t i = 0; i < 10; ++i)
    {
        game->Update(0.016);
        game->Draw();
    }

    return 0;
}
