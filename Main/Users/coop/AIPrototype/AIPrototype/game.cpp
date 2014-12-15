#include "precomp.h"
#include "Game.h"

#define DEFAULT_FRAMERATE 60

static float g_totalElapsedTime = 1.0f;
static float g_elapsedTime = 1.0f;
static BOOL g_firstUpdate = TRUE;
static LARGE_INTEGER g_performanceFrequency;
static LARGE_INTEGER g_startCounter;
static LARGE_INTEGER g_lastCounter;
static LARGE_INTEGER g_currentCounter;
float g_targetElapsedTime = 1.0f / DEFAULT_FRAMERATE;

Game::Game()
{

}

Game::~Game()
{

}

const FLOAT* Game::GetColor()
{
    return Colors::CornflowerBlue;
}

HRESULT Game::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
    if (device == nullptr || context == nullptr)
    {
        return E_INVALIDARG;
    }
    _device = device;
    _context = context;

    _spriteBatch.reset( new SpriteBatch( context ) );
    _spriteFont.reset( new SpriteFont( device, L"font11.spritefont" ) );
    _spriteFontMed.reset( new SpriteFont( device, L"font14.spritefont" ) );
    _spriteFontLarge.reset( new SpriteFont( device, L"font18.spritefont" ) );
    _vertexBatch.reset();
    _vertexBatch.reset( new PrimitiveBatch<VertexPositionColor>( context ) );

    _vertexBatchEffect.reset( new BasicEffect( device ) );
    _vertexBatchEffect->SetVertexColorEnabled(true);

    _world = XMMatrixIdentity();
    _view = XMMatrixIdentity();
    _projection = XMMatrixOrthographicOffCenterLH(0, 640, 480, 0, 0.0f, 1.0f);

    _vertexBatchEffect->SetProjection( _projection );
    _vertexBatchEffect->SetWorld(_world);
    _vertexBatchEffect->SetView( _view );

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        _vertexBatchEffect->GetVertexShaderBytecode( &shaderByteCode, &byteCodeLength );

        _device->CreateInputLayout( VertexPositionColor::InputElements,
                                    VertexPositionColor::InputElementCount,
                                    shaderByteCode, byteCodeLength,
                                    &_batchInputLayout );
    }

    OnLoadContent();
    return S_OK;
}

void Game::Cleanup()
{
    if (_batchInputLayout)
    {
        _batchInputLayout->Release();
    }
}

void Game::Update(float elapsedTime)
{
    UNREFERENCED_PARAMETER(elapsedTime);
    Keyboard::Update();
    GameTimeUpdate();
    OnUpdate(GameTimeGetElapsed());
}

void Game::Draw()
{
    _spriteBatch->Begin( /*SpriteSortMode_Deferred*/SpriteSortMode_Texture );
    OnDraw();
    _spriteBatch->End();
}

std::shared_ptr<Sprite> Game::GetSprite(std::wstring name)
{
    if (_content.find(name) == _content.end())
    {
        // Load content
        ID3D11ShaderResourceView* texture = nullptr;
        if (SUCCEEDED(CreateWICTextureFromFile(_device, _context, name.c_str(), nullptr, &texture)))
        {
            _content[name] = std::make_shared<Sprite>(texture);
        }
    }

    return _content[name];
}

void Game::DrawSprite(std::shared_ptr<Sprite> sprite, float x, float y, float rotation, FXMVECTOR color)
{
    // use XMConvertToRadians if degrees is used
    _spriteBatch->Draw( sprite->GetTexture(), XMFLOAT2(x, y ), nullptr, color, rotation );
}

void Game::DrawSprite(std::wstring sprite, float x, float y, float rotation, FXMVECTOR color)
{
    DrawSprite(GetSprite(sprite), x, y, rotation, color);
}

void Game::DrawText(std::wstring text, float x, float y, FXMVECTOR color)
{
    _spriteFont->DrawString( _spriteBatch.get(), text.c_str(), XMFLOAT2( x, y ), color );
}

void Game::DrawMedText(std::wstring text, float x, float y, FXMVECTOR color)
{
    _spriteFontMed->DrawString( _spriteBatch.get(), text.c_str(), XMFLOAT2( x, y ), color );
}

void Game::DrawLargeText(std::wstring text, float x, float y, FXMVECTOR color)
{
    _spriteFontLarge->DrawString( _spriteBatch.get(), text.c_str(), XMFLOAT2( x, y ), color );
}

void Game::DrawSprite(std::shared_ptr<Sprite> sprite, POINTF pos, float rotation, float alpha, FXMVECTOR color)
{
    FXMVECTOR colorWithAlpha = color;
    ((float*)&colorWithAlpha)[3] = alpha;
    _spriteBatch->Draw( sprite->GetTexture(), XMFLOAT2(pos.x, pos.y), nullptr, colorWithAlpha, rotation, XMFLOAT2((float)sprite->GetWidth()/2, (float)sprite->GetHeight()/2));
}

void Game::DrawSpriteWithOrigin(std::shared_ptr<Sprite> sprite, float x, float y, float rotation, float alpha, FXMVECTOR color)
{
    RECT sourceRect = {0,0,sprite->GetWidth(), sprite->GetHeight()};
    //void Draw(_In_ ID3D11ShaderResourceView* texture, XMFLOAT2 const& position, _In_opt_ RECT const* sourceRectangle, FXMVECTOR color, float rotation, XMFLOAT2 const& origin, XMFLOAT2 const& scale, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0);

    _spriteBatch->Draw(sprite->GetTexture(), XMFLOAT2(x, y), &sourceRect, color, rotation, XMFLOAT2(sprite->GetWidth()/2, sprite->GetHeight()/2), XMFLOAT2(1,1), SpriteEffects_None, 0);
}

void Game::BeginBatchedLines()
{
    _vertexBatchEffect->Apply( _context );
    _context->IASetInputLayout( _batchInputLayout );
    _vertexBatch->Begin();
}

void Game::DrawBatchedLine(Vector2 p1, Vector2 p2, FXMVECTOR color)
{
    VertexPositionColor v1( Vector3(p1.x, p1.y, 0), color );
    VertexPositionColor v2( Vector3(p2.x, p2.y, 0), color );
    _vertexBatch->DrawLine( v1, v2 );
}

void Game::EndBatchedLines()
{
    _vertexBatch->End();
}

void Game::DrawLine(Vector2 p1, Vector2 p2, FXMVECTOR color)
{
    BeginBatchedLines();
    DrawBatchedLine(p1,p2, color);
    EndBatchedLines();
}

int Game::GetSpriteWidth(std::wstring sprite)
{
    std::shared_ptr<Sprite> theSprite = GetSprite(sprite);
    if (theSprite != nullptr)
    {
        return theSprite->GetWidth();
    }

    return 0;
}

int Game::GetSpriteHeight(std::wstring sprite)
{
    std::shared_ptr<Sprite> theSprite = GetSprite(sprite);
    if (theSprite != nullptr)
    {
        return theSprite->GetHeight();
    }

    return 0;
}

int Game::GetScreenWidth()
{
    return 640;
}

int Game::GetScreenHeight()
{
    return 480;
}

void Game::GameTimeUpdate()
{
    if (g_firstUpdate)
    {
        g_firstUpdate = FALSE;

        QueryPerformanceFrequency(&g_performanceFrequency);
        QueryPerformanceCounter(&g_startCounter);
        QueryPerformanceCounter(&g_lastCounter);
    }

    QueryPerformanceCounter(&g_currentCounter);
    g_totalElapsedTime = (float)(g_currentCounter.QuadPart - g_startCounter.QuadPart) / (float)g_performanceFrequency.QuadPart;
    g_elapsedTime = (float)(g_currentCounter.QuadPart - g_lastCounter.QuadPart) / (float)g_performanceFrequency.QuadPart;
    g_lastCounter.QuadPart = g_currentCounter.QuadPart;
}

Vector2 Game::ClampToViewport(Vector2 vector)
{
    vector.x = Clamp(vector.x, 0, (float)GetScreenWidth());
    vector.y = Clamp(vector.y, 0, (float)GetScreenHeight());
    return vector;
}

float Clamp( float v, float a, float b)
{
    return (float)max(a, min(b,v));
}

float Lerp(float v1, float v2, float i)
{
    return v1 + (i * (v2 - v1));
}

float WrapAngle(float radians)
{
    while (radians < -XM_PI)
    {
        radians += XM_2PI;
    }
    while (radians > XM_PI)
    {
        radians -= XM_2PI;
    }
    return radians;
}

bool PointInCircle(Vector2 Pos, double radius, Vector2 p)
{
    double DistFromCenterSquared = (p-Pos).LengthSquared();

    if (DistFromCenterSquared < (radius*radius))
    {
    return true;
    }

  return false;
}

bool LineIntersection2D(Vector2   A,
                        Vector2   B,
                        Vector2   C, 
                        Vector2   D,
                        double&   dist,
                        Vector2&  point)
{
    double rTop = (A.y-C.y)*(D.x-C.x)-(A.x-C.x)*(D.y-C.y);
    double rBot = (B.x-A.x)*(D.y-C.y)-(B.y-A.y)*(D.x-C.x);

    double sTop = (A.y-C.y)*(B.x-A.x)-(A.x-C.x)*(B.y-A.y);
    double sBot = (B.x-A.x)*(D.y-C.y)-(B.y-A.y)*(D.x-C.x);

    if ( (rBot == 0) || (sBot == 0))
    {
        //lines are parallel
        return false;
    }

    double r = rTop/rBot;
    double s = sTop/sBot;

    if( (r > 0) && (r < 1) && (s > 0) && (s < 1) )
    {
        dist = Vector2::Distance(A,B) * r;
        point = A + r * (B - A);
        return true;
    }

    else
    {
        dist = 0;
        return false;
    }

    return false;
}

float TurnToFace(Vector2 position, Vector2 faceThis,
            float currentAngle, float turnSpeed)
{
    // consider this diagram:
    //         B 
    //        /|
    //      /  |
    //    /    | y
    //  / o    |
    // A--------
    //     x
    // 
    // where A is the position of the object, B is the position of the target,
    // and "o" is the angle that the object should be facing in order to 
    // point at the target. we need to know what o is. using trig, we know that
    //      tan(theta)       = opposite / adjacent
    //      tan(o)           = y / x
    // if we take the arctan of both sides of this equation...
    //      arctan( tan(o) ) = arctan( y / x )
    //      o                = arctan( y / x )
    // so, we can use x and y to find o, our "desiredAngle."
    // x and y are just the differences in position between the two objects.
    float x = faceThis.x - position.x;
    float y = faceThis.y - position.y;

    // we'll use the Atan2 function. Atan will calculates the arc tangent of 
    // y / x for us, and has the added benefit that it will use the signs of x
    // and y to determine what cartesian quadrant to put the result in.
    // http://msdn2.microsoft.com/en-us/library/system.math.atan2.aspx
    float desiredAngle = (float)atan2(y, x);

    // so now we know where we WANT to be facing, and where we ARE facing...
    // if we weren't constrained by turnSpeed, this would be easy: we'd just 
    // return desiredAngle.
    // instead, we have to calculate how much we WANT to turn, and then make
    // sure that's not more than turnSpeed.

    // first, figure out how much we want to turn, using WrapAngle to get our
    // result from -Pi to Pi ( -180 degrees to 180 degrees )
    float difference = WrapAngle(desiredAngle - currentAngle);

    // clamp that between -turnSpeed and turnSpeed.
    difference = Clamp(difference, -turnSpeed, turnSpeed);

    // so, the closest we can get to our target is currentAngle + difference.
    // return that, using WrapAngle again.
    return WrapAngle(currentAngle + difference);
}



float Game::GameTimeGetTotalElapsed()
{
    return g_totalElapsedTime;
}

float Game::GameTimeGetElapsed()
{
    return g_elapsedTime;
}

keyboardstate_t g_keyboardState;
mousestate_t g_mouseState;

void Keyboard::GetState(_Out_ const keyboardstate_t** state)
{
    *state = &g_keyboardState;
}

void Keyboard::Update()
{
    memcpy(g_keyboardState.Previous, g_keyboardState.Current, sizeof(g_keyboardState.Current));
    GetKeyboardState(g_keyboardState.Current);
}

void Keyboard::ClearState()
{
    ZeroMemory(&g_keyboardState.Previous, sizeof(g_keyboardState.Previous));
    ZeroMemory(&g_keyboardState.Current, sizeof(g_keyboardState.Current));
}

_Use_decl_annotations_
bool Keyboard::IsKeyDown(short key )
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) != 0;
}

_Use_decl_annotations_
bool Keyboard::IsKeyHeld(short key )
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return ((g_keyboardState.Current[key] & g_keyboardState.Previous[key]) & 0x80) != 0;
}

_Use_decl_annotations_
bool Keyboard::IsKeyJustPressed(short key)
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) != 0 && (g_keyboardState.Previous[key] & 0x80) == 0;
}

_Use_decl_annotations_
bool Keyboard::IsKeyJustReleased(short key)
{
    assert(key >= 0 && key < ARRAYSIZE(g_keyboardState.Current));
    return (g_keyboardState.Current[key] & 0x80) == 0 && (g_keyboardState.Previous[key] & 0x80) != 0;
}

void Mouse::Update()
{
    memcpy(g_mouseState.Previous, g_mouseState.Current, sizeof(g_mouseState.Current));
    memcpy(&g_mouseState.LastPoint, &g_mouseState.Point, sizeof(g_mouseState.Point));
    g_mouseState.Current[0] = GetKeyState(VK_LBUTTON);
    g_mouseState.Current[1] = GetKeyState(VK_MBUTTON);
    g_mouseState.Current[2] = GetKeyState(VK_RBUTTON);
    GetCursorPos(&g_mouseState.Point);
}

_Use_decl_annotations_
BYTE Mouse::VkeyToIndex(short vkey)
{
    switch (vkey)
    {
    case VK_LBUTTON:
        return 0;
    case VK_MBUTTON:
        return 1;
    case VK_RBUTTON:
        return 2;
    };

    return 0;
}

_Use_decl_annotations_
bool Mouse::IsButtonDown(short button )
{
    BYTE key = VkeyToIndex(button);
    assert(key >= 0 && key < ARRAYSIZE(g_mouseState.Current));
    return (g_mouseState.Current[key] & 0x80) != 0;
}

_Use_decl_annotations_
bool Mouse::IsButtonHeld(short button )
{
    BYTE key = VkeyToIndex(button);
    assert(key >= 0 && key < ARRAYSIZE(g_mouseState.Current));
    return ((g_mouseState.Current[key] & g_mouseState.Previous[key]) & 0x80) != 0;
}

_Use_decl_annotations_
bool Mouse::IsButtonJustPressed(short button)
{
    BYTE key = VkeyToIndex(button);
    assert(key >= 0 && key < ARRAYSIZE(g_mouseState.Current));
    return (g_mouseState.Current[key] & 0x80) != 0 && (g_mouseState.Previous[key] & 0x80) == 0;
}

_Use_decl_annotations_
bool Mouse::IsButtonJustReleased(short button)
{
    BYTE key = VkeyToIndex(button);
    assert(key >= 0 && key < ARRAYSIZE(g_mouseState.Current));
    return (g_mouseState.Current[key] & 0x80) == 0 && (g_mouseState.Previous[key] & 0x80) != 0;
}

POINTF Mouse::GetPosition()
{
    POINTF pos;
    pos.x = (float)g_mouseState.Point.x;
    pos.y = (float)g_mouseState.Point.y;

    return pos;
}

POINTF Mouse::GetPositionDelta()
{
    POINTF pos;
    pos.x = (float)g_mouseState.Point.x - g_mouseState.LastPoint.x;
    pos.y = (float)g_mouseState.Point.y - g_mouseState.LastPoint.y;

    return pos;
}
