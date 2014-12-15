#pragma once

class WalParser
{
public:
    WalParser(_Inout_ std::unique_ptr<uint8_t[]>& palettePcx, uint32_t size);

    void LoadWalIntoTextureIndex(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11Texture2D>& texture, uint32_t index, const std::unique_ptr<uint8_t[]>& chunk, _Out_ uint32_t* width, _Out_ uint32_t* height) const;

private:
    struct RGB
    {
        uint8_t R;
        uint8_t G;
        uint8_t B;
    };

private:
    WalParser(const WalParser&);
    WalParser& operator= (const WalParser&);

private:
    RGB _palette[256];
};
