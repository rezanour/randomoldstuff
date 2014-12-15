#include "stdafx.h"

CContentPluginAccess* CContentPluginManager::CreatePluginForFile(LPCWSTR filePath, BOOL bSupportsSave)
{
    CContentPluginAccess* pPlugin = NULL;

    WCHAR        szAppPath[MAX_PATH] = L"";
    std::wstring strAppDirectory;

    ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

    // Extract directory
    strAppDirectory = szAppPath;
    strAppDirectory = strAppDirectory.substr(0, strAppDirectory.find_last_of(L"\\"));
    
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    std::wstring actualSearch = strAppDirectory + L"\\Plugins\\*.dll";

    hFind = FindFirstFile(actualSearch.c_str(), &FindData);
    do
    {
        if (hFind == INVALID_HANDLE_VALUE)
        {
            return NULL;
        }

        std::wstring fileName = FindData.cFileName;

        if (fileName == L"." || fileName == L"..")
        {
            continue;
        }
        else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        {
            continue;
        }
        else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }
        else
        {
            BOOL bPluginSupportsFile = FALSE;
            std::wstring pluginFilePath;
            pluginFilePath = strAppDirectory + L"\\Plugins\\" + fileName;

            DEBUG_PRINT("Loading Plugin '%ws'...", pluginFilePath.c_str());
            HINSTANCE hPlugin = ::LoadLibrary(pluginFilePath.c_str());
            if (hPlugin != NULL)
            {
                pPlugin = new CContentPluginAccess(hPlugin);
                if (pPlugin != NULL)
                {
                    if (bSupportsSave ? pPlugin->CanSaveContent(filePath) : pPlugin->CanLoadContent(filePath))
                    {
                        DEBUG_PRINT("Plugin '%ws' reported that '%ws' is supported", pluginFilePath.c_str(), filePath);
                        hPlugin = NULL; // HINSTANCE is now owned by the CContentPluginAccess class instance
                        break;
                    }
                    else
                    {
                        DEBUG_PRINT("Plugin '%ws' reported that '%ws' is NOT supported", pluginFilePath.c_str(), filePath);
                        SAFE_DELETE(pPlugin);
                    }
                }
            }

            SAFE_FREELIBRARY(hPlugin);
        }
    } while (FindNextFile(hFind, &FindData));

    FindClose(hFind);

    return pPlugin;
}

/*

void CEditorDoc::EnumerateFiles(CFileNode* pRootNode, std::wstring name, bool hidden)
{
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    std::wstring actualSearch = name + L"\\Plugins\\*.dll";

    hFind = FindFirstFile(actualSearch.c_str(), &FindData);
    do
    {
        if (hFind == INVALID_HANDLE_VALUE)
        {
            return;
        }

        std::wstring fileName = FindData.cFileName;

        if (fileName == L"." || fileName == L"..")
        {
            continue;
        }
        else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN && !hidden)
        {
            continue;
        }
        else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }
        else
        {
            \\ fileName;
        }
    } while (FindNextFile(hFind, &FindData));

    FindClose(hFind);
}

*/