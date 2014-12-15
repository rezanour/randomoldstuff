#pragma once

class ScreenSystem : public BaseObject<ScreenSystem>, public IScreenSystem
{
public:
    static std::shared_ptr<ScreenSystem> Create();
    ~ScreenSystem();

    //
    // IScreenSystem
    //

    void Clear() override;

    void PushScreen(_In_ const std::shared_ptr<IScreen>& screen) override;
    void PopScreen() override;

    void RemoveScreen(_In_ const std::shared_ptr<IScreen>& screen) override;

    void InsertScreen(_In_ const std::shared_ptr<IScreen>& screen, _In_ const std::shared_ptr<IScreen>& insertAfter) override;

    void Update(_In_ const UpdateContext& context) override;

    void Draw(_In_ const DrawContext& context) override;

private:
    ScreenSystem();

private:
    std::list<std::shared_ptr<IScreen>> _screens;
    std::list<std::shared_ptr<IScreen>> _deferredScreens;

    bool _insideUpdate;
    uint32_t _popCount;
};
