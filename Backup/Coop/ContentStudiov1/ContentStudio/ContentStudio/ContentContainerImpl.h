#pragma once

class CContentContainer : public IContentContainer
{
public:
    CContentContainer();
    virtual ~CContentContainer();
    HRESULT AddModelContent(IModelContentPtr& pContent);
    IModelContentPtr GetModelContent(UINT id);
    HRESULT AddTextureContent(ITextureContentPtr& pContent);
    ITextureContentPtr GetTextureContent(UINT id);

private:
    std::vector<IModelContentPtr> m_models;
    std::vector<ITextureContentPtr> m_textures;
};

typedef std::shared_ptr<CContentContainer> ContentContainerPtr;