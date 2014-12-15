#pragma once

struct IGraphicsSystem;
struct ISpriteFont;

enum class MainMenuItemType
{
    TestGraphics = 0,
    TestAI,
    Game,
    Exit,
    LastMenuItem
};

class MainMenuScreen : public BaseObject<MainMenuScreen>, public IScreen
{
public:
    static std::shared_ptr<MainMenuScreen> Create(_In_ const std::shared_ptr<IGraphicsSystem>& graphics);

    //
    // IScreen
    //

    bool SupportsFocus() const override;
    void Update(_In_ const UpdateContext& context, _In_ bool hasFocus) override;
    void Draw(_In_ const DrawContext& context) override;

private:
    MainMenuScreen(_In_ const std::shared_ptr<IGraphicsSystem>& graphics);
    bool HandleMenuSelection(_In_ const UpdateContext& context);

private:
    std::shared_ptr<ISpriteFont> _spriteFont;
    std::shared_ptr<ISpriteFont> _spriteFontSelected;

    uint32_t _selectedItem;
    wchar_t* _items[MainMenuItemType::LastMenuItem];
    bool _hasFocus;
};
