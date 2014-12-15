#pragma once

[uuid("21ae8138-1e1c-4b63-83d6-d41fb174d760")]
struct IDirectXTextureResource : public ITextureResource
{
    virtual HRESULT CreateShaderResourceView(_In_ ID3D11Device* pDevice, _Deref_out_ ID3D11ShaderResourceView** ppShaderResourceView) = 0;
    virtual HRESULT CreateHBITMAP(HBITMAP* phBitmap) = 0;
};
