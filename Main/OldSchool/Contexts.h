#pragma once

struct IGraphicsSystem;
struct IInput;
struct IAudioSystem;
struct IScreenSystem;

struct UpdateContext
{
    float ElapsedTime;
    std::shared_ptr<IGraphicsSystem> GraphicsSystem;
    std::shared_ptr<IAudioSystem> AudioSystem;
    std::shared_ptr<IScreenSystem> ScreenSystem;
    std::shared_ptr<IInput> Input;
};

struct DrawContext
{
    std::shared_ptr<IGraphicsSystem> GraphicsSystem;
};
