#pragma once

class IContentPluginServices
{
public:
    virtual IContentContainerPtr GetContentContainer() = 0;
    virtual HRESULT CreateModelContent(IModelContentPtr* ppContent) = 0;
    virtual HRESULT CreateTextureContent(ITextureContentPtr* ppContent) = 0;
    virtual HRESULT CreateTextureContentFromFile(LPCWSTR filePath, ITextureContentPtr* ppContent) = 0;
};

typedef std::shared_ptr<IContentPluginServices> IContentPluginServicesPtr;
