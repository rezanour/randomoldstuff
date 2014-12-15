#include "StdAfx.h"
#include "ContentManager.h"

#include <CoreServices\MemoryStream.h>
#include <CoreServices\StreamHelper.h>
#include <CoreServices\PropertyBag.h>

#include <DirectXTK\DirectXMath.h>

using namespace Lucid3D;
using namespace GDK;
using namespace CoreServices;
using namespace DirectX;

// HACK: temporary for bootstrapping
typedef std::map<uint64, ContentTagPtr> ContentTagMap;
static ContentTagMap ContentTableOfContents;

typedef std::map<uint64, std::wstring> FileMap;
static FileMap FileTableOfContents;

ContentManager::ContentManager(_In_ const std::string& contentRoot)
    : _contentRoot(stde::to_wstring(contentRoot))
{
    if (_contentRoot.size() > 0 && 
        _contentRoot[_contentRoot.size() - 1] != L'\\')
    {
        _contentRoot += L"\\";
    }
}

HRESULT ContentManager::Create(_In_ const std::string& contentRoot, _Inout_ ContentManagerPtr& spContentManager)
{
    HRESULT hr = S_OK;

    ConfigurationPtr spConfiguration;
    CHECKHR(Configuration::Load(std::wstring(stde::to_wstring(contentRoot) + L"\\content.index"), &spConfiguration));

    if (FileTableOfContents.size() == 0)
    {
        size_t count = spConfiguration->GetCount();
        for (size_t i = 0; i < count; i++)
        {
            std::string id = spConfiguration->GetValueNameAt(i);
            std::string value = spConfiguration->GetStringValue(id);

            std::istringstream ss(value);
            std::string s;

            uint32 contentType;
            uint32 subContentType;
            uint64 realId;
            std::string name;
            std::string path;

            std::getline(ss, s, ',');
            ISTRUE(stde::from_string(s, contentType), E_FAIL);

            std::getline(ss, s, ',');
            ISTRUE(stde::from_string(s, subContentType), E_FAIL);

            std::getline(ss, name, ',');
            std::getline(ss, path);

            ISTRUE(stde::from_string(id, realId), E_FAIL);

            ContentTableOfContents[realId] = new ContentTag(static_cast<GDK::ContentType::Enum>(contentType), subContentType, name, realId);
            FileTableOfContents[realId] = stde::to_wstring(path);
        }
    }

    spContentManager.attach(new ContentManager(contentRoot));

EXIT
    return S_OK;
}

// IContentManager
GDK_METHODIMP ContentManager::GetContentTag(_In_ uint64 id, _Out_ IContentTag** ppContentTag)
{
    // TODO: look up the id in whatever table and hydrate a content tag for it
    if (!ppContentTag)
        return E_POINTER;

    std::map<uint64, ContentTagPtr>::iterator it = ContentTableOfContents.find(id);
    if (it == ContentTableOfContents.end())
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    *ppContentTag = it->second;
    (*ppContentTag)->AddRef();

    return S_OK;
}

GDK_METHODIMP ContentManager::RegisterRuntimeResource(_In_ GDK::IRuntimeResource* pResource)
{
    if (!pResource)
        return E_INVALIDARG;

    IRuntimeResourcePtr spResource;
    spResource.reset(pResource);
    _resources.push_back(spResource);

    return S_OK;
}

GDK_METHODIMP ContentManager::UnregisterRuntimeResource(_In_ GDK::IRuntimeResource* pResource)
{
    if (!pResource)
        return E_INVALIDARG;

    for (std::vector<IRuntimeResourcePtr>::iterator it = _resources.begin(); it != _resources.end(); it++)
    {
        if ((*it)->GetContentId() == pResource->GetContentId())
        {
            _resources.erase(it);
            break;
        }
    }

    return S_OK;
}

GDK_METHODIMP ContentManager::GetStream(_In_ uint64 id, _Deref_out_ IStream** ppStream)
{
    HRESULT hr = S_OK;

    FileMap::iterator it = FileTableOfContents.find(id);

    std::wstring contentPath(_contentRoot);
    contentPath += it->second.c_str();

    ISNOTNULL(ppStream, E_POINTER);

    ISTRUE(it != FileTableOfContents.end(), HRESULT_FROM_WIN32(ERROR_NOT_FOUND));

    DebugInfo("Mapping content id %llu to %S", id, contentPath.c_str());

    CHECKHR(FileStream::Create(contentPath.c_str(), true, ppStream));

EXIT
    return hr;
}

void ContentManager::UnloadAndReleaseAll()
{
    for (size_t i = 0; i < _resources.size(); i++)
    for (std::vector<IRuntimeResourcePtr>::iterator it = _resources.begin(); it != _resources.end(); it++)
    {
        _resources[i]->Unload();
    }

    _resources.clear();
}
