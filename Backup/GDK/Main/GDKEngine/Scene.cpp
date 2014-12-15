#include "StdAfx.h"

using GDK::ContentManager;
using GDK::ContentManagerPtr;
using GDK::Scene;
using GDK::ScenePtr;

Scene::Scene(_In_ const ContentManagerPtr& contentManager, _In_ uint64_t contentId)
    : _contentManager(contentManager), _contentId(contentId)
{
}

Scene::~Scene()
{
}

GDK_IMETHODIMP_(uint64_t) Scene::GetContentId() const
{
    return _contentId;
}

GDK_IMETHODIMP_(bool) Scene::IsLoaded() const
{
    return true;
}

GDK_IMETHODIMP_(void) Scene::Unload()
{
}

GDK_IMETHODIMP_(void) Scene::Load()
{
}
