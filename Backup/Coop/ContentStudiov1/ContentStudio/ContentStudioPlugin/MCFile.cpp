#include "stdafx.h"

HRESULT LoadMCContent(LPCWSTR filePath, IContentPluginServices* pServices)
{
    DEBUG_PRINT("Loading Content Studio model content...");
    HRESULT hr = S_OK;
    IModelContentPtr pModelContent;

    hr = pServices->CreateModelContent(&pModelContent);
    if (SUCCEEDED(hr))
    {
        hr = pModelContent->LoadFromFile(filePath);
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

HRESULT SaveMCContent(LPCWSTR filePath, IContentPluginServices* pServices)
{
    DEBUG_PRINT("Saving Content Studio model content...");
    HRESULT hr = S_OK;
    IModelContentPtr pModelContent;
    IContentContainerPtr pContentContainer;

    pContentContainer = pServices->GetContentContainer();
    if (pContentContainer != NULL)
    {
        pModelContent = pContentContainer->GetModelContent(0);
        if (pModelContent)
        {
            hr = pModelContent->SaveToFile(filePath);
        }
        else
        {
            DEBUG_PRINT("No IModelContentPtr was found, aborting save operation");
            hr = E_ABORT;
        }
    }
    else
    {
        DEBUG_PRINT("No IContentContainer was found, aborting save operation");
        hr = E_ABORT;
    }

    return hr;
}