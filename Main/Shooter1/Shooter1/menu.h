#pragma once

enum class MenuId
{
    Undefined = 0,
    MainMenu,
    GameMenu,
    OptionsMenu,
    AudioMenu,
    ControlsMenu,
    HudMenu,
    GraphicsMenu
};

typedef struct
{
    MenuId Id;
    int LastMenuItem;
} MenuState;

class MenuItem : public TrackedObject<MemoryTag::MenuItem>
{
public:
    MenuItem(_In_ int id, _In_z_ const char* text);
public:
    int id;
    std::string text;
    bool enabled;
};

class Menu : public TrackedObject<MemoryTag::Menu>
{
public:
    Menu(_In_ MenuId id, _In_z_ const char* name);
    virtual ~Menu();

    void Update();
    void Draw();
    MenuId GetId();

    virtual void OnSelected(_In_ MenuItem item) = 0;
    virtual void OnDismiss() = 0;
    virtual void OnRightSelected(_In_ MenuItem item) {}
    virtual void OnLeftSelected(_In_ MenuItem item) {}

private:
    void Next();
    void Previous();
    void Select();
    void Dismiss();

protected:
    MenuId _id;
    std::string _name;
    std::vector<MenuItem> _items;
    std::vector<XMFLOAT4> _colorTable;
    std::vector<XMFLOAT4> _disabledColorTable;
    int _selectedItem;
    SpriteFont _spriteFont;
    SingleSoundEffect _beep;
    SingleSoundEffect _boop;
};