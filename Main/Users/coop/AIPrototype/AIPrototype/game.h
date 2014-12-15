#pragma once

#include <windows.h>
#include <d3d11.h>
#include <memory>
#include <map>
#include <string>
#include <vector>

#include "SimpleMath.h"
#include "CommonStates.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "Model.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

struct RECTF
{
    FLOAT x;
    FLOAT y;
    FLOAT width;
    FLOAT height;
};

struct mousestate_t
{
    POINT Point;
    POINT LastPoint;
    short Current[3];
    short Previous[3];
};

struct keyboardstate_t
{
    BYTE Current[500];
    BYTE Previous[500];
};

class Keyboard
{
public:
   static void Update();
   static bool IsKeyDown(_In_ short key);
   static bool IsKeyHeld(_In_ short key);
   static bool IsKeyJustPressed(_In_ short key);
   static bool IsKeyJustReleased(_In_ short key);
   static void ClearState();
   static void GetState(_Out_ const keyboardstate_t** state);
};

class Mouse
{
    enum class MouseButton
    {
        Left,
        Middle,
        Right
    };

public:
    static void Update();
    static bool IsButtonDown(_In_ short button );
    static bool IsButtonHeld(_In_ short button );
    static bool IsButtonJustPressed(_In_ short button);
    static bool IsButtonJustReleased(_In_ short button);

    static POINTF GetPosition();
    static POINTF GetPositionDelta();
    static BYTE VkeyToIndex(_In_ short vkey);
};

class Sprite
{
public:
    Sprite(ID3D11ShaderResourceView* texture)
    { 
        _texture = texture;
        IntializeSpriteSize();
    }

    ~Sprite(){if (_texture) _texture->Release();}
    ID3D11ShaderResourceView* GetTexture() { return _texture; }	
    int GetWidth() { return _width;	}
    int GetHeight() { return _height; }

private:
    void IntializeSpriteSize()
    {
        ID3D11Resource* resource = nullptr;
        _texture->GetResource(&resource);
        if (resource != nullptr)
        {
            ID3D11Texture2D* texture2D = nullptr;
            if (SUCCEEDED(resource->QueryInterface(IID_ID3D11Texture2D, (void**)&texture2D)))
            {
                D3D11_TEXTURE2D_DESC desc;
                texture2D->GetDesc(&desc);
                texture2D->Release();

                _width = (int)desc.Width;
                _height = (int)desc.Height;
            }
            resource->Release();
        }
    }
    
protected:
    ID3D11ShaderResourceView* _texture;
    int _width;
    int _height;
};

// Helpers
float WrapAngle(float radians);
float Clamp( float v, float a, float b);
float Lerp(float v1, float v2, float i);
float TurnToFace(Vector2 position, Vector2 faceThis,
            float currentAngle, float turnSpeed);
bool PointInCircle(Vector2 Pos, double radius, Vector2 p);
bool LineIntersection2D(Vector2   A, Vector2   B,
                        Vector2   C, Vector2   D,
                        double& dist, Vector2&  point);

class Game
{
public:
    Game();
    ~Game();
    HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void Cleanup();
    void Update(float elapsedTime);
    void Draw();
    virtual void OnLoadContent() = 0;
    virtual void OnUpdate(float elapsedTime) = 0;
    virtual void OnDraw() = 0;
    virtual const FLOAT* GetColor();

    std::shared_ptr<Sprite> GetSprite(std::wstring name);
    void DrawSprite(std::shared_ptr<Sprite> sprite, float x, float y, float rotation = 0.0f, FXMVECTOR color = Colors::White);
    void DrawSprite(std::wstring sprite, float x, float y, float rotation = 0.0f, FXMVECTOR color = Colors::White);
    void DrawSprite(std::shared_ptr<Sprite> sprite, POINTF pos, float rotation = 0.0f, float alpha = 1.0f, FXMVECTOR color = Colors::White);
    void DrawText(std::wstring text, float x, float y, FXMVECTOR color = Colors::White);
    void DrawMedText(std::wstring text, float x, float y, FXMVECTOR color = Colors::White);
    void DrawLargeText(std::wstring text, float x, float y, FXMVECTOR color = Colors::White);

    void DrawSpriteWithOrigin(std::shared_ptr<Sprite> sprite, float x, float y, float rotation = 0.0f, float alpha = 1.0f, FXMVECTOR color = Colors::White);

    void DrawLine(Vector2 p1, Vector2 p2, FXMVECTOR color = Colors::White);

    void BeginBatchedLines();
    void DrawBatchedLine(Vector2 p1, Vector2 p2, FXMVECTOR color = Colors::White);
    void EndBatchedLines();

    int GetScreenWidth();
    int GetScreenHeight();
    int GetSpriteWidth(std::wstring sprite);
    int GetSpriteHeight(std::wstring sprite);

    void GameTimeUpdate();
    float GameTimeGetTotalElapsed();
    float GameTimeGetElapsed();

    Vector2 ClampToViewport(Vector2 vector);

protected:
    ID3D11Device* _device; 
    ID3D11DeviceContext* _context;
    ID3D11InputLayout*   _batchInputLayout;
    std::unique_ptr<SpriteBatch> _spriteBatch;
    std::unique_ptr<SpriteFont>  _spriteFont;
    std::unique_ptr<SpriteFont>  _spriteFontMed;
    std::unique_ptr<SpriteFont>  _spriteFontLarge;
    std::unique_ptr<PrimitiveBatch<VertexPositionColor>> _vertexBatch;
    std::unique_ptr<BasicEffect>                         _vertexBatchEffect;

    XMMATRIX _world;
    XMMATRIX _view;
    XMMATRIX _projection;

    std::map<std::wstring, std::shared_ptr<Sprite>> _content;
};

/*
class GameObject
{
public:
    GameObject(float x, float y, float width, float height)
    {
        boundingSphere.Center = XMFLOAT3(x, y, 0);
        boundingSphere.Radius = 6.0f;
        this->width = width;
        this->height = height;
        this->dead = false;
    }

    POINTF GetPos()
    {
        POINTF pt;
        pt.x = boundingSphere.Center.x - (width/2);
        pt.y = boundingSphere.Center.y - (height/2);
        return pt;
    }

    void SetPos(float x, float y)
    {
        boundingSphere.Center.x = x + (width/2);
        boundingSphere.Center.y = y + (height/2);
    }

    void SetPosY(float y)
    {
        boundingSphere.Center.y = y + (height/2);
    }

    bool Intersects(std::shared_ptr<GameObject> object)
    {
        return boundingSphere.Intersects(object->boundingSphere);
    }

    BoundingSphere boundingSphere;
    float width;
    float height;
    bool dead;
    float orientation;
};
*/