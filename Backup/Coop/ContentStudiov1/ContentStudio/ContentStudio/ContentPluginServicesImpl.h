#pragma once

class CContentPluginServices : public IContentPluginServices
{
public:
    CContentPluginServices();
    virtual ~CContentPluginServices();

public:
    IContentContainerPtr GetContentContainer();
    HRESULT CreateModelContent(IModelContentPtr* ppContent);
    HRESULT CreateTextureContent(ITextureContentPtr* ppContent);
    HRESULT CreateTextureContentFromFile(LPCWSTR filePath, ITextureContentPtr* ppContent);

private:
    ContentContainerPtr m_container;
};