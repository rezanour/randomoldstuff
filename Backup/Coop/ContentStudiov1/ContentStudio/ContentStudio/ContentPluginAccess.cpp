#include "stdafx.h"

CContentPluginAccess::CContentPluginAccess(HINSTANCE hInstance) :
    m_hInstance(hInstance),
    m_canLoadContent(NULL),
    m_canSaveContent(NULL),
    m_loadContent(NULL),
    m_saveContent(NULL)
{
    m_canLoadContent = (PCANLOADCONTENT) GetProcAddress(hInstance, "CanLoadContent");
    if (m_canLoadContent == NULL)
    {
        //DEBUG_PRINT("CanLoadContent is not supported by plugin");
    }

    m_canSaveContent = (PCANSAVECONTENT) GetProcAddress(hInstance, "CanSaveContent");
    if (m_canSaveContent == NULL)
    {
        //DEBUG_PRINT("CanSaveContent is not supported by plugin");
    }

    m_loadContent = (PLOADCONTENT) GetProcAddress(hInstance, "LoadContent");
    if (m_loadContent == NULL)
    {
        //DEBUG_PRINT("LoadContent is not supported by plugin");
    }

    m_saveContent = (PSAVECONTENT) GetProcAddress(hInstance, "SaveContent");
    if (m_saveContent == NULL)
    {
        //DEBUG_PRINT("SaveContent is not supported by plugin");
    }
}

CContentPluginAccess::~CContentPluginAccess()
{
    SAFE_FREELIBRARY(m_hInstance);
}

BOOL CContentPluginAccess::CanLoadContent(LPCWSTR filePath)
{
    if (m_canLoadContent != NULL)
    {
        return m_canLoadContent(filePath);
    }

    return FALSE;
}

BOOL CContentPluginAccess::CanSaveContent(LPCWSTR filePath)
{
    if (m_canSaveContent != NULL)
    {
        return m_canSaveContent(filePath);
    }

    return FALSE;
}

HRESULT CContentPluginAccess::LoadContent(LPCWSTR filePath, IContentPluginServices* pServices)
{
    if (m_loadContent != NULL)
    {
        return m_loadContent(filePath, pServices);
    }
    return E_NOTIMPL;
}

HRESULT CContentPluginAccess::SaveContent(LPCWSTR filePath, IContentPluginServices* pServices)
{
    if (m_saveContent != NULL)
    {
        return m_saveContent(filePath, pServices);
    }
    return E_NOTIMPL;
}