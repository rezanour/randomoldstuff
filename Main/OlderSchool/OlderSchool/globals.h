#pragma once

class Graphics;
class Game;

struct Globals
{
    // Config
    static const char GameTitle[];
    static uint32_t ScreenWidth;
    static uint32_t ScreenHeight;

    // Objects
    static Graphics* Graphics;
    static Game* Game;
};
