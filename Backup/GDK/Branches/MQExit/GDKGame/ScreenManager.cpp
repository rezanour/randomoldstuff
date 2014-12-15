#include "StdAfx.h"

#include "Screen.h"
#include "ScreenManager.h"
#include "GameApplication.h"
#include "SubsystemManager.h"

using namespace GDK;

ScreenManager::ScreenManager(_In_ GameApplication* pApplication)
    : _pApplication(pApplication)
{
    Assert(pApplication);
}

ScreenManager::~ScreenManager()
{
    _pApplication = nullptr;
}

HRESULT ScreenManager::StartLoadingScreen(_In_ uint64 contentId)
{
    HRESULT hr = S_OK;

    ScreenPtr spScreen;
    FATAL(Screen::CreateScreen(this, 1, spScreen));

    _screens[contentId] = spScreen;

EXIT
    return hr;
}

HRESULT ScreenManager::ChangeScreen(_In_ uint64 contentId)
{
    HRESULT hr = S_OK;

    ScreenPtr nextScreen;

    ScreenMap::iterator it = _screens.find(contentId);
    if (it != _screens.end())
    {
        nextScreen = it->second;
    }
    else
    {
        FATAL(Screen::CreateScreen(this, contentId, nextScreen));
        _screens[contentId] = nextScreen;
    }

    // TODO: Wait for screen to finish loading (in case it's not done loading yet)
    _activeScreen = nextScreen;

EXIT
    return hr;
}

HRESULT ScreenManager::RemoveScreen(_In_ uint64 contentId)
{
    HRESULT hr = S_OK;

    ScreenMap::iterator it = _screens.find(contentId);
    if (it != _screens.end())
    {
        _screens.erase(it);
    }
    else
    {
        DebugWarning("Requested removal of a screen that the screen manager doesn't have.");
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    return hr;
}

HRESULT ScreenManager::Update()
{
    HRESULT hr = S_OK;

    if (_activeScreen)
    {
        hr = _activeScreen->Update();
    }

    if (SUCCEEDED(hr))
    {
        hr = GetRenderer()->Render();
    }

    return hr;
}

void ScreenManager::ExitApplication()
{
    if (_pApplication)
    {
        _pApplication->Exit();
    }
}

IRenderer* ScreenManager::GetRenderer() const
{
    return _pApplication->GetSubsystemManager()->GetRenderer();
}

ContentManagerPtr ScreenManager::GetContentManager() const
{
    return _pApplication->GetContentManager();
}

