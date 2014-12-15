#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_SCENE_H_
#define _GDK_SCENE_H_

namespace GDK
{
    class Scene;
    typedef stde::ref_ptr<Scene> ScenePtr;

    class Scene : public RefCounted<IContent>
    {
    public:
        // IContent
        GDK_IMETHOD_(uint64_t) GetContentId() const;
        GDK_IMETHOD_(bool) IsLoaded() const;
        GDK_IMETHOD_(void) Unload();
        GDK_IMETHOD_(void) Load();

    private:
        // allow content manager to factory out Scenes
        friend class ContentManager;

        Scene(_In_ const ContentManagerPtr& contentManager, _In_ uint64_t contentId);
        ~Scene();

        // Content
        ContentManagerPtr _contentManager;
        uint64_t _contentId;

        // Scene
        std::vector<stde::ref_ptr<IGameObject>> _gameObjects;
        stde::ref_ptr<IRendererScene> _renderer;
    };

} // GDK

#endif // _GDK_SCENE_H_
