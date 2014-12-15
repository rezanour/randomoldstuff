#pragma once

#include <Platform.h>

namespace GDK
{
    struct GameTime
    {
        GameTime() : deltaTime(0), totalTime(0)
        {}

        GameTime(_In_ float deltaTime, _In_ float totalTime) :
            deltaTime(deltaTime), totalTime(totalTime)
        {}

        float deltaTime;
        float totalTime;
    };
}
