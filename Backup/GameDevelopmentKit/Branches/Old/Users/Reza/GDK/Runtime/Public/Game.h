#pragma once

#include "Platform.h"
#include "Content.h"
#include "GraphicsDevice.h"

namespace GDK
{
    // forward declarations
    class InputDevice;
    class AudioDevice;

    struct GameCreationParameters
    {
        std::shared_ptr<Content> Content;
        std::shared_ptr<GraphicsDevice> GraphicsDevice;
        std::shared_ptr<AudioDevice> AudioDevice;
        std::shared_ptr<InputDevice> InputDevice;
    };

    //
    // Base class for a game. Each title will derive from
    // this and customize it to fit their needs.
    //
    class Game : public NonCopyable
    {
    public:
        const std::wstring& GetName() const;

        // These are not virtual because derived games shouldn't override
        // them completely. Instead, specific hooks are provided as protected
        // virtual methods for derived classes to plug in with.
        void Update(_In_ double elapsedSeconds);
        void Draw();

    protected:
        Game(_In_ const std::wstring& name, _In_ const GameCreationParameters& parameters);

        virtual void OnBeginUpdate(_In_ double elapsedSeconds);
        virtual void OnEndUpdate(_In_ double elapsedSeconds);

    private:
        std::wstring _name;
        std::shared_ptr<Content> _content;
        std::shared_ptr<GraphicsDevice> _graphicsDevice;
        std::shared_ptr<AudioDevice> _audioDevice;
        std::shared_ptr<InputDevice> _inputDevice;
    };
}
