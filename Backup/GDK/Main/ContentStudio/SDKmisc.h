//--------------------------------------------------------------------------------------
// File: SDKMisc.h
//
// Various helper functionality that is shared between SDK samples
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef SDKMISC_H
#define SDKMISC_H


//-----------------------------------------------------------------------------
// Resource cache for textures, fonts, meshs, and effects.  
// Use DXUTGetGlobalResourceCache() to access the global cache
//-----------------------------------------------------------------------------

enum DXUTCACHE_SOURCELOCATION
{
    DXUTCACHE_LOCATION_FILE,
    DXUTCACHE_LOCATION_RESOURCE
};

struct DXUTCache_Texture
{
    DXUTCACHE_SOURCELOCATION Location;
    WCHAR   wszSource[MAX_PATH];
    HMODULE hSrcModule;
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    UINT MiscFlags;
    union
    {
        DWORD Usage9;
        D3D11_USAGE Usage11;
    };
    union
    {
        D3DFORMAT Format9;
        DXGI_FORMAT Format;
    };
    union
    {
        D3DPOOL Pool9;
        UINT CpuAccessFlags;
    };
    union
    {
        D3DRESOURCETYPE Type9;
        UINT BindFlags;
    };
    IDirect3DBaseTexture9* pTexture9;
    ID3D11ShaderResourceView* pSRV11;

            DXUTCache_Texture()
            {
                pTexture9 = NULL;
                pSRV11 = NULL;
            }
};

struct DXUTCache_Font : public D3DXFONT_DESC
{
    ID3DXFont* pFont;
};

struct DXUTCache_Effect
{
    DXUTCACHE_SOURCELOCATION Location;
    WCHAR wszSource[MAX_PATH];
    HMODULE hSrcModule;
    DWORD dwFlags;
    ID3DXEffect* pEffect;
};


class CDXUTResourceCache
{
public:
                            ~CDXUTResourceCache();

    HRESULT                 CreateTextureFromFile( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pSrcFile,
                                                   LPDIRECT3DTEXTURE9* ppTexture );
    HRESULT                 CreateTextureFromFile( LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile,
                                                   LPDIRECT3DTEXTURE9* ppTexture );
    HRESULT                 CreateTextureFromFile( ID3D11Device* pDevice, ID3D11DeviceContext *pContext, LPCTSTR pSrcFile,
                                                   ID3D11ShaderResourceView** ppOutputRV, bool bSRGB=false );
    HRESULT                 CreateTextureFromFile( ID3D11Device* pDevice, ID3D11DeviceContext *pContext, LPCSTR pSrcFile,
                                                   ID3D11ShaderResourceView** ppOutputRV, bool bSRGB=false );
    HRESULT                 CreateTextureFromFileEx( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pSrcFile, UINT Width,
                                                     UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format,
                                                     D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey,
                                                     D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
                                                     LPDIRECT3DTEXTURE9* ppTexture );
    HRESULT                 CreateTextureFromFileEx( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LPCTSTR pSrcFile,
                                                     D3DX11_IMAGE_LOAD_INFO* pLoadInfo, ID3DX11ThreadPump* pPump,
                                                     ID3D11ShaderResourceView** ppOutputRV, bool bSRGB );
    HRESULT                 CreateTextureFromResource( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule,
                                                       LPCTSTR pSrcResource, LPDIRECT3DTEXTURE9* ppTexture );
    HRESULT                 CreateTextureFromResourceEx( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule,
                                                         LPCTSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels,
                                                         DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter,
                                                         DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo,
                                                         PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture );
    HRESULT                 CreateCubeTextureFromFile( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pSrcFile,
                                                       LPDIRECT3DCUBETEXTURE9* ppCubeTexture );
    HRESULT                 CreateCubeTextureFromFileEx( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pSrcFile, UINT Size,
                                                         UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                                         DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey,
                                                         D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
                                                         LPDIRECT3DCUBETEXTURE9* ppCubeTexture );
    HRESULT                 CreateCubeTextureFromResource( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule,
                                                           LPCTSTR pSrcResource,
                                                           LPDIRECT3DCUBETEXTURE9* ppCubeTexture );
    HRESULT                 CreateCubeTextureFromResourceEx( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule,
                                                             LPCTSTR pSrcResource, UINT Size, UINT MipLevels,
                                                             DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter,
                                                             DWORD MipFilter, D3DCOLOR ColorKey,
                                                             D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
                                                             LPDIRECT3DCUBETEXTURE9* ppCubeTexture );
    HRESULT                 CreateVolumeTextureFromFile( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pSrcFile,
                                                         LPDIRECT3DVOLUMETEXTURE9* ppVolumeTexture );
    HRESULT                 CreateVolumeTextureFromFileEx( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pSrcFile, UINT Width,
                                                           UINT Height, UINT Depth, UINT MipLevels, DWORD Usage,
                                                           D3DFORMAT Format, D3DPOOL Pool, DWORD Filter,
                                                           DWORD MipFilter, D3DCOLOR ColorKey,
                                                           D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
                                                           LPDIRECT3DVOLUMETEXTURE9* ppTexture );
    HRESULT                 CreateVolumeTextureFromResource( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule,
                                                             LPCTSTR pSrcResource,
                                                             LPDIRECT3DVOLUMETEXTURE9* ppVolumeTexture );
    HRESULT                 CreateVolumeTextureFromResourceEx( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule,
                                                               LPCTSTR pSrcResource, UINT Width, UINT Height,
                                                               UINT Depth, UINT MipLevels, DWORD Usage,
                                                               D3DFORMAT Format, D3DPOOL Pool, DWORD Filter,
                                                               DWORD MipFilter, D3DCOLOR ColorKey,
                                                               D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
                                                               LPDIRECT3DVOLUMETEXTURE9* ppVolumeTexture );
    HRESULT                 CreateFont( LPDIRECT3DDEVICE9 pDevice, UINT Height, UINT Width, UINT Weight,
                                        UINT MipLevels, BOOL Italic, DWORD CharSet, DWORD OutputPrecision,
                                        DWORD Quality, DWORD PitchAndFamily, LPCTSTR pFacename, LPD3DXFONT* ppFont );
    HRESULT CreateFontIndirect( LPDIRECT3DDEVICE9 pDevice, CONST D3DXFONT_DESC *pDesc, LPD3DXFONT *ppFont );
    HRESULT                 CreateEffectFromFile( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pSrcFile,
                                                  const D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags,
                                                  LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect,
                                                  LPD3DXBUFFER* ppCompilationErrors );
    HRESULT                 CreateEffectFromResource( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule,
                                                      LPCTSTR pSrcResource, const D3DXMACRO* pDefines,
                                                      LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool,
                                                      LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors );

public:
    HRESULT                 OnCreateDevice( IDirect3DDevice9* pd3dDevice );
    HRESULT                 OnResetDevice( IDirect3DDevice9* pd3dDevice );
    HRESULT                 OnLostDevice();
    HRESULT                 OnDestroyDevice();

protected:
    friend CDXUTResourceCache& WINAPI DXUTGetGlobalResourceCache();
    friend HRESULT WINAPI   DXUTInitialize3DEnvironment();
    friend HRESULT WINAPI   DXUTReset3DEnvironment();
    friend void WINAPI      DXUTCleanup3DEnvironment( bool bReleaseSettings );

                            CDXUTResourceCache()
                            {
                            }

    CGrowableArray <DXUTCache_Texture> m_TextureCache;
    CGrowableArray <DXUTCache_Effect> m_EffectCache;
    CGrowableArray <DXUTCache_Font> m_FontCache;
};

CDXUTResourceCache& WINAPI DXUTGetGlobalResourceCache();

//--------------------------------------------------------------------------------------
// Shared code for samples to ask user if they want to use a REF device or quit
//--------------------------------------------------------------------------------------
void WINAPI DXUTDisplaySwitchingToREFWarning( DXUTDeviceVersion ver );

//--------------------------------------------------------------------------------------
// Tries to finds a media file by searching in common locations
//--------------------------------------------------------------------------------------
HRESULT WINAPI DXUTFindDXSDKMediaFileCch( __out_ecount(cchDest) WCHAR* strDestPath,
                                          __in int cchDest, 
                                          __in LPCWSTR strFilename );
HRESULT WINAPI DXUTSetMediaSearchPath( LPCWSTR strPath );
LPCWSTR WINAPI DXUTGetMediaSearchPath();


//--------------------------------------------------------------------------------------
// Returns a view matrix for rendering to a face of a cubemap.
//--------------------------------------------------------------------------------------
D3DXMATRIX WINAPI DXUTGetCubeMapViewMatrix( DWORD dwFace );


//--------------------------------------------------------------------------------------
// Simple helper stack class
//--------------------------------------------------------------------------------------
template <class T> class CDXUTStack
{
private:
    UINT m_MemorySize;
    UINT m_NumElements;
    T* m_pData;

    bool    EnsureStackSize( UINT64 iElements )
    {
        if( m_MemorySize > iElements )
            return true;

        T* pTemp = new T[ ( size_t )( iElements * 2 + 256 ) ];
        if( !pTemp )
            return false;

        if( m_NumElements )
        {
            CopyMemory( pTemp, m_pData, ( size_t )( m_NumElements * sizeof( T ) ) );
        }

        if( m_pData ) delete []m_pData;
        m_pData = pTemp;
        return true;
    }

public:
            CDXUTStack()
            {
                m_pData = NULL; m_NumElements = 0; m_MemorySize = 0;
            }
            ~CDXUTStack()
            {
                if( m_pData ) delete []m_pData;
            }

    UINT    GetCount()
    {
        return m_NumElements;
    }
    T       GetAt( UINT i )
    {
        return m_pData[i];
    }
    T       GetTop()
    {
        if( m_NumElements < 1 )
            return NULL;

        return m_pData[ m_NumElements - 1 ];
    }

    T       GetRelative( INT i )
    {
        INT64 iVal = m_NumElements - 1 + i;
        if( iVal < 0 )
            return NULL;
        return m_pData[ iVal ];
    }

    bool    Push( T pElem )
    {
        if( !EnsureStackSize( m_NumElements + 1 ) )
            return false;

        m_pData[m_NumElements] = pElem;
        m_NumElements++;

        return true;
    }

    T       Pop()
    {
        if( m_NumElements < 1 )
            return NULL;

        m_NumElements --;
        return m_pData[m_NumElements];
    }
};


#endif
