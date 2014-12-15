#pragma once

class IContentContainer
{
public:
    virtual HRESULT AddModelContent(IModelContentPtr& pContent) = 0;
    virtual IModelContentPtr GetModelContent(UINT id) = 0;
    virtual HRESULT AddTextureContent(ITextureContentPtr& pContent) = 0;
    virtual ITextureContentPtr GetTextureContent(UINT id) = 0;
};

typedef std::shared_ptr<IContentContainer> IContentContainerPtr;