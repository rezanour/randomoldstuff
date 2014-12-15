#include "StdAfx.h"

#include <stde\com_ptr.h>
#include <DirectXTK\DirectXMath.h>

#include "Screen.h"
#include "ScreenManager.h"
#include <stde\conversion.h>

#include <CoreServices\StreamHelper.h>

#include "ObjectModel\GameObject.h"

#include <string>

using namespace Lucid3D;
using namespace DirectX;
using namespace GDK;
using namespace CoreServices;

HRESULT Screen::CreateScreen(_In_ ScreenManager* pScreenManager, _In_ uint64 contentId, _Inout_ ScreenPtr& spScreen)
{
    HRESULT hr = S_OK;

    IRendererScenePtr spScene;

    CHECKHR(pScreenManager->GetRenderer()->CreateScene(&spScene));

    spScreen.reset(new Screen(pScreenManager, spScene, contentId));
    ISNOTNULL(spScreen, E_OUTOFMEMORY);

    CHECKHR(spScreen->StartLoadingData());

EXIT
    return hr;
}

Screen::Screen(_In_ ScreenManager* pScreenManager, _In_ IRendererScenePtr spScene, _In_ uint64 contentId)
    : _pScreenManager(pScreenManager), _contentId(contentId), _isLoaded(false), _spScene(spScene)
{
    Assert(pScreenManager);
    Assert(spScene);
    _spContentManager.reset(pScreenManager->GetContentManager()); 
}

Screen::~Screen()
{
    _pScreenManager = nullptr;
}

HRESULT Screen::StartLoadingData()
{
    HRESULT hr = S_OK;

    if (!_isLoaded)
    {
        stde::com_ptr<IStream> spStream;
        CHECKHR(_spContentManager->GetStream(_contentId, &spStream));
        CHECKHR(ParseScreen(spStream));
        _isLoaded = true;
    }

EXIT
    return hr;
}

HRESULT Screen::Update()
{
    return S_OK;
}

HRESULT Screen::ParseScreen(_In_ stde::com_ptr<IStream> spStream)
{
    HRESULT hr = S_OK;

    ConfigurationPtr spProperties;
    std::string gameObjectIds;
    std::vector<uint64> ids;

    CHECKHR(Configuration::Load(spStream, &spProperties));

    // TODO: First, parse out any templates

    // Next, parse out instances
    gameObjectIds = spProperties->GetStringValue("GameObjectIds");

    // seperate on ,
    {
        std::string token;
        std::stringstream ss(gameObjectIds);
        while (ss.good())
        {
            std::getline(ss, token, ',');
            uint64 id = 0;
            ISTRUE(stde::from_string(token, id), E_FAIL);
            ids.push_back(id);
        }
    }

    for (size_t i = 0; i < ids.size(); ++i)
    {
        GameObjectPtr spGameObject;
        CHECKHR(GameObject::Load(this, ids[i], spGameObject));
        _gameObjects.push_back(spGameObject);
    }

EXIT
    if (FAILED(hr))
    {
        // if we failed, clear out the scene
        _gameObjects.clear();
    }

    return hr;
}

