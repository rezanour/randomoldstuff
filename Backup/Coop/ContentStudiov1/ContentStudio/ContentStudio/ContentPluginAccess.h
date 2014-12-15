#pragma once

class CContentPluginAccess
{
public:
    CContentPluginAccess(HINSTANCE hInstance);
    virtual ~CContentPluginAccess();
    
    BOOL CanLoadContent(LPCWSTR filePath);
    BOOL CanSaveContent(LPCWSTR filePath);
    HRESULT LoadContent(LPCWSTR filePath, IContentPluginServices* pServices);
    HRESULT SaveContent(LPCWSTR filePath, IContentPluginServices* pServices);

private:
    HINSTANCE m_hInstance;
    PCANLOADCONTENT m_canLoadContent;
    PCANSAVECONTENT m_canSaveContent;
    PLOADCONTENT m_loadContent;
    PSAVECONTENT m_saveContent;
};