#include "Precomp.h"
#include "ScreenSystem.h"
#include "ScreenSystemImpl.h"
#include "Screen.h"

std::shared_ptr<IScreenSystem> CreateScreenSystem()
{
    return ScreenSystem::Create();
}

std::shared_ptr<ScreenSystem> ScreenSystem::Create()
{
    return std::shared_ptr<ScreenSystem>(new ScreenSystem);
}

ScreenSystem::ScreenSystem() :
    _insideUpdate(false),
    _popCount(0)
{
}

ScreenSystem::~ScreenSystem()
{
    Clear();
}

void ScreenSystem::Clear()
{
    _screens.clear();
}

_Use_decl_annotations_
void ScreenSystem::PushScreen(const std::shared_ptr<IScreen>& screen)
{
    if (!_insideUpdate)
    {
        _screens.push_front(screen);
    }
    else
    {
        _deferredScreens.push_front(screen);
    }
}

void ScreenSystem::PopScreen()
{
    if (!_insideUpdate)
    {
        _screens.pop_front();
    }
    else
    {
        _popCount++;
    }
}

_Use_decl_annotations_
void ScreenSystem::RemoveScreen(const std::shared_ptr<IScreen>& screen)
{
    auto it = std::find(std::begin(_screens), std::end(_screens), screen);
    if (it != std::end(_screens))
    {
        _screens.erase(it);
    }
}

_Use_decl_annotations_
void ScreenSystem::InsertScreen(const std::shared_ptr<IScreen>& screen, const std::shared_ptr<IScreen>& insertAfter)
{
    auto it = std::find(std::begin(_screens), std::end(_screens), insertAfter);
    if (it == std::end(_screens))
    {
        throw std::exception();
    }

    _screens.insert(it, screen);
}

_Use_decl_annotations_
void ScreenSystem::Update(const UpdateContext& context)
{
    _insideUpdate = true;
    bool handledFocus = false;

    for (auto screen : _screens)
    {
        bool hasFocus = false;
        if (!handledFocus && screen->SupportsFocus())
        {
            hasFocus = true;
            handledFocus = true;
        }

        screen->Update(context, hasFocus);
    }

    _insideUpdate = false;

    // Pop any pending screens
    while (_popCount != 0)
    {
        PopScreen();
        _popCount--;
    }

    // Push any pending screens
    for (auto deferredScreen : _deferredScreens)
    {
        PushScreen(deferredScreen);
    }

    _deferredScreens.clear();
}

_Use_decl_annotations_
void ScreenSystem::Draw(const DrawContext& context)
{
    for (auto it = _screens.rbegin(); it != _screens.rend(); ++it)
    {
        (*it)->Draw(context);
    }
}
