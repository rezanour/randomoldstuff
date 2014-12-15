#include "Quake2Game.h"

using namespace GDK;

namespace Quake2
{
    std::shared_ptr<Quake2Game> Quake2Game::Create(_In_ const GDK::Game::CreateParameters& parameters)
    {
        return std::shared_ptr<Quake2Game>(GDKNEW Quake2Game(parameters));
    }

    Quake2Game::Quake2Game(_In_ const GDK::Game::CreateParameters& parameters) :
        Game(parameters)
    {
    }
}
