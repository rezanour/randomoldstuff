#include "Precomp.h"

using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Content {

// IContentFactory
HRESULT GDKAPI ContentArchiveFactory::CreateContent(_In_ IStream* data, _COM_Outptr_ IContent** content)
{
    MODULE_GUARD_BEGIN

    CHECKHR(Make<ContentArchive>(data).CopyTo(content));

    MODULE_GUARD_END
}

ContentArchive::ContentArchive(_In_ IStream* data) :
    _fileBased(false)
{
    _stream = data;
    uint32_t  contentArchiveVersion = 0;
    unsigned long bytesRead = 0;

    // Read content archive header to see what type of archive it is
    _stream->Read(&contentArchiveVersion, sizeof(contentArchiveVersion), &bytesRead);
    _fileBased = (contentArchiveVersion == FILEBASED_CONTENT_ARCHIVE_VERSION);
    
    if (_fileBased)
    {
        // Read the root path.  This value is dynamic so it can be of any length.
        // Read until no more content file is present.
        char ch = 0;
        while(SUCCEEDED(_stream->Read(&ch, sizeof(ch), &bytesRead)) && bytesRead != 0)
        {
            wchar_t wch;
            mbtowc(&wch, &ch, sizeof(ch));
            _rootPath += wch;
        }

        if (!IsAbsolutePath(_rootPath))
        {
            const wchar_t* name = nullptr;
            if (SUCCEEDED(data->GetName(&name)))
            {
                std::wstring fullPath = DirectoryRootFromPath(name);
                fullPath.append(L"\\");
                fullPath.append(_rootPath);                
                _rootPath.clear();
                _rootPath = fullPath;
            }
        }
       
        _rootPath.append(L"\\"); // trailing slash makes concatination of resource identifiers easy
    }

    // Set stream back to beginning
    _stream->SeekTo(STREAM_SEEK_SET, 0);
}

bool ContentArchive::IsAbsolutePath(std::wstring& path)
{
    size_t pos = path.find_first_of(L":");
    if (pos == std::wstring::npos)
    {
        return false;
    }

    return true;
}

std::wstring ContentArchive::DirectoryRootFromPath(std::wstring path)
{
    std::wstring root;
    size_t pos = path.find_last_of(L"\\");
    if (pos != std::wstring::npos)
    {
        root = path.substr(0, pos);
    }
    else
    {
        root = path;
    }

    return root;
}

ContentArchive::~ContentArchive()
{
}

// IContent
HRESULT GDKAPI ContentArchive::GetGeometryResource(_In_z_ const wchar_t* name, _COM_Outptr_ IGeometryResource** resource)
{
    HRESULT hr = S_OK;
    ComPtr<GDK::IStream> resourceStream;
    ComPtr<IGeometryResourceEdit> geometryResourceEdit;
    ComPtr<IResourceFactory> resourceFactory;
    
    hr = CreateResourceFactory(&resourceFactory);
    if (SUCCEEDED(hr))
    {
        if (_fileBased)
        {        
            std::wstring pathToResource = _rootPath;
            pathToResource.append(name);
        
            hr = CreateFileStream(pathToResource.c_str(), true, &resourceStream);            
        }
        else
        {
            // Seek to the archive data
            hr = SeekToResource(name, &resourceStream);
        }

        // Create and initialize geometry resource
        if (SUCCEEDED(hr))
        {        
            hr = resourceFactory->CreateGeometryResource(&geometryResourceEdit);
            if (SUCCEEDED(hr))
            {
                hr = geometryResourceEdit->Load(resourceStream.Get());
            }

            if (SUCCEEDED(hr))
            {
                hr = geometryResourceEdit->SetName(name);
            }

            if (SUCCEEDED(hr))
            {
                hr = geometryResourceEdit.CopyTo(resource);                
            }        
        }
    }
    
    return hr;
}

HRESULT ContentArchive::SeekToResource(_In_z_ const wchar_t* name, _COM_Outptr_ IStream** stream)
{
    HRESULT hr = S_OK;
    unsigned long bytesRead = 0;
    // Set stream back to beginning
    CONTENT_ARCHIVE_FILEHEADER fileHeader = {0};
    CONTENT_ARCHIVE_ENTRY entry = {0};
    bool entryFound = false;

    // Read the archive header
    hr = _stream->SeekTo(STREAM_SEEK_SET, 0);
    if (SUCCEEDED(hr))
    {
        hr = _stream->Read(&fileHeader, sizeof(fileHeader), &bytesRead);
    }

    // Find the matching resource
    if (SUCCEEDED(hr))
    {
        for (unsigned __int64 i = 0; i < fileHeader.FileCount; i++)
        {
            std::wstring entryName;
            ZeroMemory(&entry, sizeof(entry));

            hr = _stream->Read(&entry, sizeof(entry), &bytesRead);
            if (SUCCEEDED(hr) && bytesRead > 0)
            {
                CopyCharArrayAsWideString(entry.Name, ARRAYSIZE(entry.Name), entryName);
                if (entryName.compare(name) == 0)
                {
                    entryFound = true;
                    break;
                }
            }
        }
    }

    // If found, seek to resource data and return stream at that position
    if (entryFound)
    {
        hr = _stream->SeekTo(STREAM_SEEK_SET, entry.Offset);
        if (SUCCEEDED(hr))
        {
            hr = _stream.CopyTo(stream);
        }
    }

    return hr;
}

HRESULT ContentArchive::CopyCharArrayAsWideString(char* chars, int numchars, std::wstring& str)
{
    if (!chars || numchars == 0)
    {
        return E_INVALIDARG;
    }

    for (int i = 0; i < numchars; i++)
    {
        wchar_t wch;
        mbtowc(&wch, &chars[i], sizeof(chars[i]));
        str += wch;
    }

    return S_OK;
}

HRESULT GDKAPI ContentArchive::GetTextureResource(_In_z_ const wchar_t* name, _COM_Outptr_ ITextureResource** resource)
{
    HRESULT hr = S_OK;
    ComPtr<GDK::IStream> resourceStream;
    ComPtr<ITextureResourceEdit> textureResourceEdit;
    ComPtr<IResourceFactory> resourceFactory;
    
    hr = CreateResourceFactory(&resourceFactory);
    if (SUCCEEDED(hr))
    {
        if (_fileBased)
        {        
            std::wstring pathToResource = _rootPath;
            pathToResource.append(name);
        
            hr = CreateFileStream(pathToResource.c_str(), true, &resourceStream);            
        }
        else
        {
            // Seek to the archive data
            hr = SeekToResource(name, &resourceStream);
        }

        // Create and initialize texture resource
        if (SUCCEEDED(hr))
        {        
            hr = resourceFactory->CreateTextureResource(&textureResourceEdit);
            if (SUCCEEDED(hr))
            {
                hr = textureResourceEdit->Load(resourceStream.Get());
            }

            if (SUCCEEDED(hr))
            {
                hr = textureResourceEdit->SetName(name);
            }

            if (SUCCEEDED(hr))
            {
                hr = textureResourceEdit.CopyTo(resource);
            }        
        }
    }
    
    return hr;
}

HRESULT GDKAPI ContentArchive::GetWorldResource(_In_z_ const wchar_t* name, _COM_Outptr_ IWorldResource** resource)
{
    HRESULT hr = S_OK;
    ComPtr<GDK::IStream> resourceStream;
    ComPtr<IWorldResourceEdit> worldResourceEdit;
    ComPtr<IResourceFactory> resourceFactory;
    
    hr = CreateResourceFactory(&resourceFactory);
    if (SUCCEEDED(hr))
    {
        if (_fileBased)
        {
            std::wstring pathToResource = _rootPath;
            pathToResource.append(name);

            hr = CreateFileStream(pathToResource.c_str(), true, &resourceStream);
        }
        else
        {
            // Seek to the archive data
            hr = SeekToResource(name, &resourceStream);
        }

        // Create and initialize world resource
        if (SUCCEEDED(hr))
        {
            hr = resourceFactory->CreateWorldResource(&worldResourceEdit);
            if (SUCCEEDED(hr))
            {
                hr = worldResourceEdit->Load(resourceStream.Get());
            }

            if (SUCCEEDED(hr))
            {
                hr = worldResourceEdit->SetName(name);
            }

            if (SUCCEEDED(hr))
            {
                hr = worldResourceEdit.CopyTo(resource);
            }
        }
    }

    return hr;
}

HRESULT GDKAPI ContentArchive::CreateResourceFactory(_COM_Outptr_ IResourceFactory** factory)
{   
    if (!factory)
    {
        return E_INVALIDARG;
    }

    return Make<ResourceFactory>().CopyTo(factory);
}
} // Content
} // GDK
