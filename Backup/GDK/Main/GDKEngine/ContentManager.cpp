#include "StdAfx.h"

using GDK::ContentManager;
using GDK::ContentManagerPtr;

GDK_METHOD_(ContentManagerPtr) ContentManager::Create(_In_ const wchar_t* contentRoot)
{
    UNREFERENCED_PARAMETER(contentRoot);
    ContentManagerPtr contentManager;
    contentManager.attach(new ContentManager);
    return contentManager;
}

ContentManager::ContentManager()
{
}

ContentManager::~ContentManager()
{
}
