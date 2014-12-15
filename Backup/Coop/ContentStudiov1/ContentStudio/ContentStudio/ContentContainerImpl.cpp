#include "stdafx.h"

CContentContainer::CContentContainer()
{

}

CContentContainer::~CContentContainer()
{

}

HRESULT CContentContainer::AddModelContent(IModelContentPtr& pContent)
{
    HRESULT hr = S_OK;

    m_models.push_back(pContent);

    return hr;
}

IModelContentPtr CContentContainer::GetModelContent(UINT id)
{
    if (m_models.size() == 0)
        return nullptr;

    if (m_models.size() <= id)
        return nullptr;

    return m_models[id];
}

HRESULT CContentContainer::AddTextureContent(ITextureContentPtr& pContent)
{
    HRESULT hr = S_OK;

    m_textures.push_back(pContent);

    return hr;
}

ITextureContentPtr CContentContainer::GetTextureContent(UINT id)
{
    if (m_textures.size() == 0)
        return nullptr;

    if (m_textures.size() <= id)
        return nullptr;

    return m_textures[id];
}
