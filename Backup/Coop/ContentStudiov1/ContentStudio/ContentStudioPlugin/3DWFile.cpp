#include "stdafx.h"

HRESULT Load3dwContent(LPCWSTR filePath, IContentPluginServices* pServices)
{
    DEBUG_PRINT("Loading 3D World Studio content...");

    HRESULT hr = S_OK;
    Raw3DWFile file;
    IModelContentPtr pModelContent;

    hr = pServices->CreateModelContent(&pModelContent);
    if (SUCCEEDED(hr))
    {
        //hr = file.LoadAsOneMesh(filePath, pModelContent, pServices);
        hr = file.Load(filePath, pModelContent, pServices);
        if (SUCCEEDED(hr))
        {
            IContentContainerPtr pContent = pServices->GetContentContainer();
            if (pContent)
            {
                hr = pContent->AddModelContent(pModelContent);
            }
        }
    }
    return hr;
}