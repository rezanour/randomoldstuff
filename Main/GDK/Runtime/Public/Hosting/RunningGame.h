#pragma once

#include <Platform.h>

namespace GDK
{
    enum class UpdateResult;
    struct DeviceContext;
    struct GameTime;
    class GameWorldContent;

    struct IRunningGame
    {
        virtual const std::wstring& GetName() const = 0;
        virtual UpdateResult Update(_In_ const GameTime& gameTime) = 0;
        virtual void Draw() = 0;
    };

    namespace Game
    {
        std::shared_ptr<IRunningGame> Create(_In_ const DeviceContext& deviceContext);
        std::shared_ptr<IRunningGame> Create(_In_ const DeviceContext& deviceContext, _In_ const std::shared_ptr<GameWorldContent>& initialWorld);
    }
}
