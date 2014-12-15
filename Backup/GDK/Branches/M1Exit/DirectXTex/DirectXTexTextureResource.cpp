//-------------------------------------------------------------------------------------
// DirectXTexTR.cpp
//  
// DirectX Texture Library - Texture Resource (TextureResource) file format reader/writer
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Cooper Partin and Reza Nouri. All rights reserved.
//-------------------------------------------------------------------------------------

#include "stdafx.h"
#include <strsafe.h>
#include "directxtexp.h"

#include "scoped.h"
#include <GDK\Tools\textureresource.h>

namespace DirectX
{
    unsigned __int64 CreateUniqueId()
    {
        unsigned __int64 uniqueId = 0;
    
        GUID guid;
        CoCreateGuid(&guid);

        uniqueId = (unsigned __int64)guid.Data1 << 32;
        uniqueId |= (unsigned __int64)guid.Data2 << 16;
        uniqueId |= guid.Data3;

        return uniqueId;
    }


    //-------------------------------------------------------------------------------------
    // Load a TextureResource file from stream
    //-------------------------------------------------------------------------------------
    HRESULT LoadFromTextureResourceStream( IStream* pStream, TexMetadata* /*metadata*/, ScratchImage& image )
    {
        if ( !pStream )
            return E_INVALIDARG;

        HRESULT hr = S_OK;
        ULONG cbRead = 0;
        image.Release();

        TEXTURE_RESOURCE_FILEHEADER trf = {0};
        hr = pStream->Read(&trf, sizeof(trf), &cbRead);
        if (FAILED(hr))
        {
            return hr;
        }

        if (trf.header.Version != TEXTURE_RESOURCE_VERSION)
        {
            return E_INVALIDARG;
        }
        
        Image* imageArray = new Image[trf.metadata.arraySize];
        ZeroMemory(imageArray, sizeof(Image) * trf.metadata.arraySize);

        if (imageArray != nullptr)
        {
            for (size_t i = 0; i < trf.metadata.arraySize; i++)
            {
                TEXTURE_RESOURCE_IMAGE tri = {0};
                hr = pStream->Read(&tri, sizeof(tri), &cbRead);
                if (SUCCEEDED(hr))
                {
                    imageArray[i].format     = (DXGI_FORMAT)tri.format;
                    imageArray[i].height     = tri.height;
                    imageArray[i].rowPitch   = tri.rowPitch;
                    imageArray[i].slicePitch = tri.slicePitch;
                    imageArray[i].width      = tri.width;
                    imageArray[i].pixels     = new uint8_t[tri.slicePitch];
                    hr = pStream->Read(imageArray[i].pixels, (ULONG)tri.slicePitch, &cbRead);
                    if (FAILED(hr))
                    {
                        break;
                    }
                }
            }

            if (SUCCEEDED(hr))
            {
                hr = image.InitializeArrayFromImages(imageArray, trf.metadata.arraySize);

                for (size_t i = 0; i < trf.metadata.arraySize; i++)
                {
                    if (imageArray[i].pixels != nullptr)
                    {
                        delete [] imageArray[i].pixels;
                        imageArray[i].pixels = nullptr;
                    }
                }
            }

            delete [] imageArray;
        }

        return hr;
    }

    //-------------------------------------------------------------------------------------
    // Save a TextureResource file to stream
    //-------------------------------------------------------------------------------------
    HRESULT SaveToTextureResourceStream(const char* name, const Image* images, size_t nimages, const TexMetadata& metadata, DWORD flags, IStream* pStream )
    {
        if ( !pStream )
            return E_INVALIDARG;

        if (flags != (DWORD)TR_FLAGS::TR_FLAGS_NONE)
            return E_INVALIDARG;

        HRESULT hr = S_OK;
        ULONG cbWritten = 0;

        // write header
        TEXTURE_RESOURCE_FILEHEADER trf = {0};
        trf.header.Version     = TEXTURE_RESOURCE_VERSION;
        trf.header.Id          = metadata.uniqueId;
        StringCchCopyA( trf.header.Name, ARRAYSIZE(trf.header.Name), name );

        trf.metadata.arraySize = metadata.arraySize;
        trf.metadata.depth     = metadata.depth;
        trf.metadata.dimension = (TEXTURE_RESOURCE_DIMENSION)metadata.dimension;
        trf.metadata.format    = (TEXTURE_RESOURCE_FORMAT)metadata.format;
        trf.metadata.height    = metadata.height;
        trf.metadata.mipLevels = metadata.miscFlags;
        trf.metadata.miscFlags = metadata.miscFlags;
        trf.metadata.width     = metadata.width;
        
        hr = pStream->Write(&trf, sizeof(trf), &cbWritten);
        if (FAILED(hr))
        {
            return hr;
        }

        // Write each image found
        for (size_t i = 0; i < nimages; i++)
        {
            // Write image header
            TEXTURE_RESOURCE_IMAGE tri = {0};
            tri.format     = (TEXTURE_RESOURCE_FORMAT)images[i].format;
            tri.height     = images[i].height;
            tri.width      = images[i].width;
            tri.rowPitch   = images[i].rowPitch;
            tri.slicePitch = images[i].slicePitch;
            tri.cbPixels   = (uint32_t)images[i].slicePitch; // use slicePitch or cbPixels?
            tri.pixels     = nullptr; // used only in the runtime usage
            
            // Write image header
            hr = pStream->Write(&tri, sizeof(tri), &cbWritten);
            if (FAILED(hr))
            {
                return hr;
            }

            //size_t rowPitch, slicePitch;
            //ComputePitch( metadata.format, metadata.width, metadata.height, rowPitch, slicePitch, CP_FLAGS_NONE );


            // Write pixel data
            hr = pStream->Write(images[i].pixels, tri.cbPixels, &cbWritten);
            if (FAILED(hr))
            {
                return hr;
            }
        }

        return hr;
    }

}; // namespace