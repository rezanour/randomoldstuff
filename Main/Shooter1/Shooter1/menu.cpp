#include "precomp.h"
#include "menu.h"

// Holds cached menu selection state
std::map<MenuId, int> menuSelectionState;

_Use_decl_annotations_
MenuItem::MenuItem(int id, const char* text) :
    id(id),
    text(text),
    enabled(true)
{

}

_Use_decl_annotations_
Menu::Menu(MenuId id, const char* name) :
    _id(id),
    _name(name),
    _spriteFont(GetGame().GetSystemContent().GetSpriteFont("menufont.36pt.spritefont")),
    _beep(GetGame().GetSystemContent().GetSoundEffect("beep.wav")),
    _boop(GetGame().GetSystemContent().GetSoundEffect("boop.wav"))
{
    _selectedItem = menuSelectionState[_id];
    _colorTable.push_back(XMFLOAT4(Colors::White)); // all menus come with default white
}

Menu::~Menu()
{
    menuSelectionState[_id] = _selectedItem;
}

void Menu::Draw()
{
    POINT pt = POINT {0,0};

    // Draw menu name
    _spriteFont.DrawUsingEmbeddedMarkup(pt, _colorTable.data(), _colorTable.size(), "* %s *",_name.c_str());

    for (int i = 0; i < _items.size(); i++)
    {
        pt.y += _spriteFont.GetLineSpacing();

        if (i != _selectedItem)
        {
            // Draw menu item
            if (_items[i].enabled)
            {
                _spriteFont.DrawUsingEmbeddedMarkup(pt, _colorTable.data(), _colorTable.size(), "  %s", _items[i].text.c_str());
            }
            else
            {
                // Draw disabled menu item
                const XMFLOAT4 disabledColor[] = { XMFLOAT4(Colors::Gray) };
                _spriteFont.DrawUsingEmbeddedMarkup(pt, disabledColor, _countof(disabledColor), "  %s", _items[i].text.c_str());
            }
        }
        else
        {
            // Draw menu item selected
            _spriteFont.DrawUsingEmbeddedMarkup(pt, _colorTable.data(), _colorTable.size(), "- %s", _items[i].text.c_str());
        }
    }
}

void Menu::Update()
{
    if (GetInput().IsMenuSelectPressed())
    {
        _beep.Play();
        Select();
    }

    if (GetInput().IsMenuDownPressed())
    {
        _beep.Play();
        Next();
    }

    if (GetInput().IsMenuUpPressed())
    {
        _beep.Play();
        Previous();
    }

    if (GetInput().IsMenuDismissPressed())
    {
        _boop.Play();
        Dismiss();
    }

    if (GetInput().IsMenuRightPressed())
    {
        _boop.Play();
        OnRightSelected(_items[_selectedItem]);
    }

    if (GetInput().IsMenuLeftPressed())
    {
        _boop.Play();
        OnLeftSelected(_items[_selectedItem]);
    }
}

MenuId Menu::GetId()
{
    return _id;
}

void Menu::Next()
{
    _selectedItem++;
    if (_selectedItem >= _items.size())
    {
        _selectedItem = 0;
    }

    // If the menu item is disabled, automatically move to
    // select the next item
    if (!_items[_selectedItem].enabled)
    {
        Next();
    }
}

void Menu::Previous()
{
    _selectedItem--;
    if (_selectedItem < 0)
    {
        _selectedItem = std::max(((int)_items.size()) - 1, 0);
    }

    // If the menu item is disabled, automatically move to
    // select the previous item
    if (!_items[_selectedItem].enabled)
    {
        Previous();
    }
}

void Menu::Select()
{
    OnSelected(_items[_selectedItem]);
}

void Menu::Dismiss()
{
    OnDismiss();
}
