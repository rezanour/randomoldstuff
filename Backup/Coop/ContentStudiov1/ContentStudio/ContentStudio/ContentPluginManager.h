#pragma once

class CContentPluginManager
{
public:
    static CContentPluginAccess* CreatePluginForFile(LPCWSTR filePath, BOOL bSupportsSave);
};