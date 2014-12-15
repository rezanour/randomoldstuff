#pragma once

class ISpriteFontContent
{
public:
    virtual HRESULT SaveToFile(LPCWSTR filePath) = 0;
    virtual HRESULT LoadFromFile(LPCWSTR filePath) = 0;
    virtual HRESULT LoadFromLOGFONT(LOGFONT* plogFont) = 0;
    virtual HRESULT GetSpriteFontName(std::wstring& name) = 0;
    virtual HRESULT SetSpriteFontName(std::wstring& name) = 0;
    virtual HRESULT GetTextureData(BYTE** ppData, DWORD* pcbSize) = 0;
};

typedef std::shared_ptr<ISpriteFontContent> ISpriteFontContentPtr;