#pragma once

#include "TextureContent.h"
#include "ModelContent.h"
#include "ContentContainer.h"
#include "ContentPluginServices.h"

extern "C"
{
    //__declspec(dllexport) BOOL __stdcall CanLoadContent(LPCWSTR filePath);
    //__declspec(dllexport) BOOL __stdcall CanSaveContent(LPCWSTR filePath);
    //__declspec(dllexport) HRESULT __stdcall LoadContent(LPCWSTR inputFilePath, IContentPluginServices* pServices);
    //__declspec(dllexport) HRESULT __stdcall SaveContent(LPCWSTR outputFilePath, IContentPluginServices* pServices);
};

typedef BOOL (__stdcall *PCANLOADCONTENT)(LPCWSTR);
typedef BOOL (__stdcall *PCANSAVECONTENT)(LPCWSTR);
typedef HRESULT (__stdcall *PLOADCONTENT)(LPCWSTR, IContentPluginServices*);
typedef HRESULT (__stdcall *PSAVECONTENT)(LPCWSTR, IContentPluginServices*);
