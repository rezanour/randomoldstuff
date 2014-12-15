#pragma once

#include <GDK.h>

namespace Quake2
{
    class Quake2Game : public GDK::Game
    {
    public:
        static std::shared_ptr<Quake2Game> Create(_In_ const GDK::Game::CreateParameters& parameters);

    private:
        Quake2Game(_In_ const GDK::Game::CreateParameters& parameters);
    };
}
