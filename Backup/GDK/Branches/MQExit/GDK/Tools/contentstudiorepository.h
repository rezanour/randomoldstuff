#pragma once

#include <windows.h>
#include <GDK\Tools\textureresource.h>
#include <GDK\Tools\spritefontresource.h>
#include <GDK\Tools\geometryresource.h>

[uuid("2ED5FFDC-6FDA-44B6-B26F-8DB47B5F76B9")]
struct IContentStudioRepository : public IUnknown
{
    virtual HRESULT GetTextureResource(__int64 id, ITextureResource** ppResource) = 0;
    virtual HRESULT GetGeometryResource(__int64 id, IGeometryResource** ppResource) = 0;

    virtual HRESULT AddItem(IUnknown* pItem, __int64& id) = 0;

    virtual HRESULT GetTotalItems(size_t& numItems) = 0;
    virtual HRESULT GetItemByIndex(int index, IUnknown** ppItem) = 0;
    virtual HRESULT GetItemType(__int64 id, int& itemType) = 0;
    virtual HRESULT GetItemName(__int64 id, WCHAR* pName, int cchName) = 0;
};
