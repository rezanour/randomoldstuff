#pragma once

class CSpriteFontTextRenderer
{
public:
    CSpriteFontTextRenderer();
    virtual ~CSpriteFontTextRenderer();
    HRESULT Initialize(int pointSize, LPCWSTR faceName);
    void DrawText(DirectX::SpriteBatch& spriteBatch, int x, int y, LPCWSTR text, DirectX::XMVECTOR color = DirectX::Colors::White);
    void Clear();
    int LineHeight();
    int MeasureText(LPCWSTR text);
private:
    HRESULT CreateSpriteFont(int pointSize, LPCWSTR faceName, ISpriteFontResource** ppResource);
    stde::com_ptr<ID3D11ShaderResourceView> _sprite;
    std::map<CHAR, RECT> _glyphtionary;
    int _lineHeight;
};