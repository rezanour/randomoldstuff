// A Screen is the primary unit of execution in the Lucid engine. This is a session or context
#pragma once

#include "GDKTypes.h"
#include "ContentManager.h"
#include "ObjectModel\GameObject.h"

namespace GDK
{
    class Screen;
    typedef std::shared_ptr<Screen> ScreenPtr;

    class ScreenManager;

    class Screen : stde::non_copyable
    {
    public:
        ~Screen();

        static HRESULT CreateScreen(_In_ ScreenManager* pScreenManager, _In_ uint64 contentId, _Inout_ ScreenPtr& spScreen);

        bool IsLoaded() const { return _isLoaded; }

        HRESULT Update();

        ContentManagerPtr GetContentManager() const { return _spContentManager; }
        IRendererScenePtr GetScene() const { return _spScene; }

    private:
        Screen(_In_ ScreenManager* pScreenManager, _In_ IRendererScenePtr spScene, _In_ uint64 contentId);

        HRESULT StartLoadingData();
        HRESULT ParseScreen(_In_ stde::com_ptr<IStream> spStream);

        // Owner and metadata
        ScreenManager* _pScreenManager;
        ContentManagerPtr _spContentManager;
        uint64 _contentId;
        bool _isLoaded;

        // Subsystems
        IRendererScenePtr _spScene;

        // Scene
        std::vector<GameObjectPtr> _gameObjects;
    };
}

