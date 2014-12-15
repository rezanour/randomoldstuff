#pragma once

struct IScreen;
struct UpdateContext;
struct DrawContext;

struct __declspec(novtable) IScreenSystem
{
    virtual void Clear() = 0;

    virtual void PushScreen(_In_ const std::shared_ptr<IScreen>& screen) = 0;
    virtual void PopScreen() = 0;

    virtual void RemoveScreen(_In_ const std::shared_ptr<IScreen>& screen) = 0;

    virtual void InsertScreen(_In_ const std::shared_ptr<IScreen>& screen, _In_ const std::shared_ptr<IScreen>& insertAfter) = 0;

    virtual void Update(_In_ const UpdateContext& context) = 0;

    virtual void Draw(_In_ const DrawContext& context) = 0;
};

std::shared_ptr<IScreenSystem> CreateScreenSystem();
