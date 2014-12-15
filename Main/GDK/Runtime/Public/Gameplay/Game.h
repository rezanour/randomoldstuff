#pragma once

#include <Platform.h>

namespace GDK
{
    struct IGameWorld;
    struct GameTime;
    struct DeviceContext;
    class Matrix;
    enum class UpdateResult;

    //
    // Games implement this to help drive the experience
    //
    struct IGame
    {
        virtual void OnWorldChanged(_In_ const std::shared_ptr<IGameWorld>& gameWorld) = 0;

        virtual UpdateResult OnUpdateBegin(_In_ const GameTime& gameTime) = 0;
        virtual UpdateResult OnUpdateEnd(_In_ const GameTime& gameTime) = 0;

        virtual void OnDrawBegin(_Out_ Matrix* view, _Out_ Matrix* projection) = 0;
        virtual void OnDrawEnd(_In_ const Matrix& view, _In_ const Matrix& projection) = 0;
    };

    struct GameInfo
    {
        std::wstring name;
        std::wstring firstWorld;
        std::shared_ptr<IGame> (*onCreate)(_In_ const DeviceContext& deviceContext);
    };

    namespace Game
    {
        void RegisterGame(_In_ const GameInfo& gameInfo);
    }
}
