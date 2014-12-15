#include "precomp.h"
#include <Block3DRenderer\block3d.h>
#include "input.h"
#include "level.h"

static uint32_t ScreenWidth = 1280;
static uint32_t ScreenHeight = 720;
static const wchar_t GameTitle[] = L"Block3DTest";

#ifdef ENABLE_VR
static const float CameraTurnSpeed = 0.03f;
static const float CameraMoveSpeed = 1.5f;
static const float CameraRunSpeed = 3.0f;
#else
static const float CameraTurnSpeed = 0.05f;
static const float CameraMoveSpeed = 2.0f;
static const float CameraRunSpeed = 5.0f;
#endif

static const float PlayerRadius = 8.0f;
static const float Epsilon = 0.0001f;
static const float OneMinusEpsilon = 1.0f - Epsilon;

static HINSTANCE g_instance;
static HWND g_hwnd;
static ComPtr<ID3D11Device> g_device;
static ComPtr<ID3D11DeviceContext> g_context;
static ComPtr<IDXGISwapChain> g_swapChain;
static ComPtr<ID3D11RenderTargetView> g_renderTargetView;
static ComPtr<ID3D11DepthStencilView> g_depthStencilView;
static ComPtr<ID3D11ShaderResourceView> g_wallsSRV;
static ComPtr<ID3D11ShaderResourceView> g_spritesSRV;
static XMFLOAT2 g_position;
static float g_angle;
static XMFLOAT4X4 g_view;
static bool g_running;
static int16_t g_map[64*64];

static void CreateGameWindow();
static void CreateGraphics();
static void LoadTextures();
static void DestroyGraphics();
static LRESULT CALLBACK WndProc(_In_ HWND hwnd, _In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam);
static void UpdateViewMatrix();
static void TurnCamera(const XMFLOAT2& input);
static void MoveCamera(const XMFLOAT2& input);
static void CheckAction();
static bool IsCellEmpty(int cellX, int cellY);
static int HandleMoveWithCollision(const XMFLOAT2& movement);
static int GetCellInFront(const XMFLOAT2& position, const XMFLOAT2& direction);

struct cellinfo_t
{
    int x;
    int y;
};

bool g_enableCellTracking = false;
std::vector<cellinfo_t> checkedCells;

#define TEST_RAYCAST

#ifdef TEST_RAYCAST
const float CastDistance = 4096.0f;
#else
const float CastDistance = 32.0f;
#endif

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    InputStartup();

    VRInit();

    try
    {
        g_instance = instance;
        CreateGameWindow();

        CreateGraphics();
        LoadTextures();

        std::unique_ptr<IBlock3DRenderer> renderer(CreateBlock3DRenderer(g_device.Get(), g_wallsSRV.Get(), g_spritesSRV.Get(), /* useLinearFiltering */ false));

        renderer->SetLevelColors(XMFLOAT3(0.3f, 0.3f, 0.3f), XMFLOAT3(0.3f, 0.3f, 0.3f));
#ifdef TEST_RAYCAST
        // Create an empty map containing no walls.  We only want to track cells checked, and
        // empty checks need to return true.
        for (uint32_t i = 0; i < 64 * 64; ++i)
        {
            g_map[i] = -1;
        }
#else
        std::unique_ptr<Level> level(new Level(L"C:\\GDK\\Wolf3DRaw"));
        const int16_t* p = level->GetLevel(0);
        for (uint32_t i = 0; i < 64 * 64; ++i)
        {
            g_map[i] = *p < 150 ? *p : -1;
            ++p;
        }
#endif
        renderer->SetWallLayout(g_map);

        ShowWindow(g_hwnd, SW_SHOW);
        UpdateWindow(g_hwnd);

        g_position = XMFLOAT2(2048 + 32, 2048 + 32);
        XMFLOAT2 block(2048 - 45, 2048 + 32);

        XMFLOAT4X4 projection;
        XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovRH(XMConvertToRadians(60), ScreenWidth / (float)ScreenHeight, 0.1f, 8192.0f));

        D3D11_VIEWPORT viewport = {};
        viewport.Width = (float)ScreenWidth;
        viewport.Height = (float)ScreenHeight;
        viewport.MaxDepth = 1.0f;

        LARGE_INTEGER start, end, frequency;
        double spriteDrawTime = 0.0f;

        QueryPerformanceFrequency(&frequency);

        auto& input = GetInput();

        MSG msg = {};
        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                input.Tick();

                if (input.IsQuitButtonPressed())
                {
                    // Quit the game
                    break;
                }

                g_running = input.IsRunButtonDown();

                TurnCamera(input.GetLookVector());
                MoveCamera(input.GetMovementVector());
                UpdateViewMatrix();

                if (input.IsActionButtonPressed())
                {
                    // Raycast debugging is only performed on action presses.
                    // The casting code is shared for movement and we only
                    // want to log cells during an action, not for every cast.

                    // Clear debug cells and enable tracking
                    checkedCells.clear();
                    g_enableCellTracking = true;

                    // Perform raycast
                    CheckAction();

                    // disable tracking
                    g_enableCellTracking = false;
                }

#ifdef ENABLE_VR
                VRBeginFrame(g_device.Get(), g_swapChain.Get());

                for (uint32_t i = 0; i < 2; ++i)
                {
                    XMFLOAT4X4 view;
                    VRBeginEye(i, XMFLOAT3(g_position.x, 32.0f, g_position.y), g_angle, &viewport, &view, &projection);

                    renderer->Begin(viewport, view, projection, false);
#else
                static const float clearColor[] = { 0, 0, 0, 1 };
                g_context->ClearRenderTargetView(g_renderTargetView.Get(), clearColor);
                g_context->ClearDepthStencilView(g_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

                renderer->Begin(viewport, g_view, projection, true);
#endif
                    QueryPerformanceCounter(&start);
#ifdef TEST_RAYCAST
                    // Render any cells that have been collected.
                    // The sprite chosen is a green barrel (or whatever sprite #3 ends up being)
                    for (size_t c = 0; c < checkedCells.size(); c++)
                    {
                        renderer->DrawSprite(XMFLOAT2(checkedCells[c].x * 64.0f + 32.0f, checkedCells[c].y * 64.0f + 32.0f), 3);
                    }
#else

                    for (uint32_t y = 0; y < 64; y += 4)
                    {
                        for (uint32_t x = 0; x < 64; x += 4)
                        {
                            renderer->DrawSprite(XMFLOAT2(x * 64.0f + 32.0f, y * 64.0f + 32.0f), (y * 64 + x) % 200);
                        }
                    }
#endif
                    QueryPerformanceCounter(&end);
                    spriteDrawTime = (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;

                    renderer->End();
#ifdef ENABLE_VR
                    VREndEye();
                }

                VREndFrame();
#else
                g_swapChain->Present(1, 0);
#endif
            }
        }
    }
    catch (const std::exception&)
    {
    }

    InputShutdown();
    DestroyGraphics();

    VRShutdown();

    return 0;
}

void CreateGameWindow()
{
    WNDCLASSEX wcx = {};
    wcx.cbSize = sizeof(wcx);
#ifdef _ARM_
    wcx.hbrBackground = 0;
#else
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
#endif
    wcx.hInstance = g_instance;
    wcx.lpfnWndProc = WndProc;
    wcx.lpszClassName = GameTitle;

    if (!RegisterClassEx(&wcx))
    {
        throw std::exception();
    }

    DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    RECT rc = {};
    if (VREnabled())
    {
        rc = VRGetLocation();
        ScreenWidth = rc.right - rc.left;
        ScreenHeight = rc.bottom - rc.top;
    }
    else
    {
        rc.right = ScreenWidth;
        rc.bottom = ScreenHeight;
    }
    AdjustWindowRect(&rc, style, FALSE);

    g_hwnd = CreateWindow(GameTitle, GameTitle, style, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, g_instance, nullptr);
    if (!g_hwnd)
    {
        throw std::exception();
    }
}

void CreateGraphics()
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width = ScreenWidth;
    scd.BufferDesc.Height = ScreenHeight;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = g_hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    UINT flags = 0;

#if defined(_DEBUG)
    //flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel = VREnabled() ? D3D_FEATURE_LEVEL_11_0 : D3D_FEATURE_LEVEL_9_1;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        flags, &featureLevel, 1, D3D11_SDK_VERSION, &scd, &g_swapChain, &g_device, nullptr, &g_context);
    if (FAILED(hr)) throw std::exception();

    ComPtr<ID3D11Texture2D> texture;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
    if (FAILED(hr)) throw std::exception();

    hr = g_device->CreateRenderTargetView(texture.Get(), nullptr, &g_renderTargetView);
    if (FAILED(hr)) throw std::exception();

    D3D11_TEXTURE2D_DESC td = {};
    texture->GetDesc(&td);

    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    hr = g_device->CreateTexture2D(&td, nullptr, &texture);
    if (FAILED(hr)) throw std::exception();

    hr = g_device->CreateDepthStencilView(texture.Get(), nullptr, &g_depthStencilView);
    if (FAILED(hr)) throw std::exception();

    g_context->OMSetRenderTargets(1, g_renderTargetView.GetAddressOf(), g_depthStencilView.Get());
}

void LoadTextures()
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) throw std::exception();

    try
    {
        ComPtr<IWICImagingFactory> factory;
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
        if (FAILED(hr)) throw std::exception();

        ComPtr<IWICBitmapDecoder> decoder;
        hr = factory->CreateDecoderFromFilename(L"walls.png", nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
        if (FAILED(hr)) throw std::exception();

        ComPtr<IWICBitmapFrameDecode> frame;
        hr = decoder->GetFrame(0, &frame);
        if (FAILED(hr)) throw std::exception();

        uint32_t width, height;
        hr = frame->GetSize(&width, &height);
        if (FAILED(hr)) throw std::exception();

        std::unique_ptr<uint32_t[]> buffer(new uint32_t[width * height]);
        hr = frame->CopyPixels(nullptr, sizeof(uint32_t) * width, sizeof(uint32_t) * width * height, (uint8_t*)buffer.get());
        if (FAILED(hr)) throw std::exception();

        D3D11_TEXTURE2D_DESC td = {};
        td.ArraySize = 1;
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        td.Height = height;
        td.Width = width;
        td.MipLevels = 1;
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA init = {};
        init.pSysMem = buffer.get();
        init.SysMemPitch = sizeof(uint32_t) * width;
        init.SysMemSlicePitch = init.SysMemPitch * height;

        ComPtr<ID3D11Texture2D> texture;
        hr = g_device->CreateTexture2D(&td, &init, &texture);
        if (FAILED(hr)) throw std::exception();

        hr = g_device->CreateShaderResourceView(texture.Get(), nullptr, &g_wallsSRV);
        if (FAILED(hr)) throw std::exception();

        hr = factory->CreateDecoderFromFilename(L"sprites.png", nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
        if (FAILED(hr)) throw std::exception();

        hr = decoder->GetFrame(0, &frame);
        if (FAILED(hr)) throw std::exception();

        hr = frame->GetSize(&width, &height);
        if (FAILED(hr)) throw std::exception();

        buffer.reset(new uint32_t[width * height]);
        hr = frame->CopyPixels(nullptr, sizeof(uint32_t) * width, sizeof(uint32_t) * width * height, (uint8_t*)buffer.get());
        if (FAILED(hr)) throw std::exception();

        td.Height = height;
        td.Width = width;

        init.pSysMem = buffer.get();
        init.SysMemPitch = sizeof(uint32_t) * width;
        init.SysMemSlicePitch = init.SysMemPitch * height;

        hr = g_device->CreateTexture2D(&td, &init, &texture);
        if (FAILED(hr)) throw std::exception();

        hr = g_device->CreateShaderResourceView(texture.Get(), nullptr, &g_spritesSRV);
        if (FAILED(hr)) throw std::exception();
    }
    catch (const std::exception&)
    {
        CoUninitialize();
        throw;
    }

    CoUninitialize();
}

void DestroyGraphics()
{
    g_depthStencilView = nullptr;
    g_renderTargetView = nullptr;
    g_spritesSRV = nullptr;
    g_swapChain = nullptr;
    g_wallsSRV = nullptr;
    g_context = nullptr;
    g_device = nullptr;
}

_Use_decl_annotations_
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void UpdateViewMatrix()
{
    XMMATRIX rotation = XMMatrixRotationY(g_angle);
    XMStoreFloat4x4(&g_view, XMMatrixLookToRH(XMVectorSet(g_position.x, 32, g_position.y, 1), rotation.r[2], rotation.r[1]));
}

void TurnCamera(const XMFLOAT2& input)
{
    g_angle -= input.x * CameraTurnSpeed;
}

void MoveCamera(const XMFLOAT2& input)
{
    XMMATRIX rotation = XMMatrixRotationY(g_angle);

    if (VREnabled())
    {
        XMFLOAT4 orientation(VRGetOrientation());
        XMMATRIX headRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&orientation));
        rotation = headRotation * rotation;

        XMVECTOR forward = XMVector3Cross(rotation.r[0], XMVectorSet(0, 1, 0, 1));
        XMVECTOR right = XMVector3Cross(XMVectorSet(0, 1, 0, 1), forward);
        rotation.r[0] = right;
        rotation.r[2] = forward;
    }

    XMFLOAT3 movement3;
    XMStoreFloat3(&movement3, (rotation.r[0] * -input.x + rotation.r[2] * input.y) * (g_running ? CameraRunSpeed : CameraMoveSpeed));

    XMFLOAT2 movement(movement3.x, movement3.z);
    if (movement.x != 0 || movement.y != 0)
    {
        HandleMoveWithCollision(movement);
    }
}

void CheckAction()
{
    XMMATRIX rotation = XMMatrixRotationY(g_angle);

    if (VREnabled())
    {
        XMFLOAT4 orientation(VRGetOrientation());
        XMMATRIX headRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&orientation));
        rotation = headRotation * rotation;

        XMVECTOR forward = XMVector3Cross(rotation.r[0], XMVectorSet(0, 1, 0, 1));
        XMVECTOR right = XMVector3Cross(XMVectorSet(0, 1, 0, 1), forward);
        rotation.r[0] = right;
        rotation.r[2] = forward;
    }

    XMFLOAT3 dir;
    XMStoreFloat3(&dir, rotation.r[2] * CastDistance);
    int cell = GetCellInFront(g_position, XMFLOAT2(dir.x, dir.z));
    if (cell >= 0)
    {
        wchar_t message[50] = {};
        swprintf_s(message, L"Clicked on Wall: %d\n", g_map[cell]);
        OutputDebugString(message);
    }
}

bool IsCellEmpty(int cellX, int cellY)
{
#ifdef TEST_RAYCAST
    // If cell tracking is enabled, record every cell checked with this
    // call.
    if (g_enableCellTracking)
    {
        cellinfo_t cellInfo = { cellX, cellY };
        checkedCells.push_back(cellInfo);
    }
#endif
    return g_map[cellY * 64 + cellX] < 0;
}

typedef int (*pfnDDAReachedEnd)(const XMFLOAT2& position);
typedef int (*pfnDDACollision)(const XMFLOAT2& position, int cellX, int cellY, const XMFLOAT2& movement, float remainingDistance, bool xHit);

int RaycastDDA(const XMFLOAT2& position, float radius, const XMFLOAT2& movement, pfnDDAReachedEnd pfnReachedEnd, pfnDDACollision pfnCollision)
{
    float totalDistance = sqrtf(movement.x * movement.x + movement.y * movement.y);
    if (totalDistance == 0.0f)
    {
        return -1;
    }

    XMFLOAT2 norm(movement.x / totalDistance, movement.y / totalDistance);

    // Distance to the next cell along each direction
    float dx = 0.0f, dy = 0.0f;

    // Signed step we should take to advance to the next point in each direction
    float stepDx = 0.0f, stepDy = 0.0f;

    // Real distance (along actual move direction) to the next point in each direction
    float distX = 0.0f, distY = 0.0f;

    // Total distance traveled
    float distance = 0.0f;

    // Cell location of starting position
    int cellX = (int)g_position.x / 64;
    int cellY = (int)g_position.y / 64;

    //
    // Depending on the signs of the components of the movement vector,
    // we determine which direction on each axis we're moving and set up
    // the DDA variables based on that. We also "grow" the cells towards the
    // player by the player radius to handle the player being a circle.
    //
    // By clamping initial distX and distY to 0, we ensure we don't bounce backwards
    // if we start slightly embedded in a wall, which can happen due to floating point rounding error
    //
    if (fabs(norm.x) < Epsilon)
    {
        // Moving straight up and down. By setting distX to really large,
        // we guarantee x-axis won't get picked as a way to try traversing
        distX = FLT_MAX;
    }
    else if (norm.x > 0)
    {
        // Moving to the right
        dx = (cellX + 1) * 64.0f - g_position.x - radius;
        stepDx = 64.0f;
        distX = std::max(0.0f, dx / norm.x);
    }
    else if (norm.x < 0)
    {
        // Moving to the left
        dx = cellX * 64.0f - g_position.x + radius;
        stepDx = -64.0f;
        distX = std::max(0.0f, dx / norm.x);
    }

    if (fabs(norm.y) < Epsilon)
    {
        // Moving straight side to side. By setting distY to really large,
        // we guarantee y-axis won't get picked to traverse.
        distY = FLT_MAX;
    }
    else if (norm.y > 0)
    {
        // Moving forward
        dy = (cellY + 1) * 64.0f - g_position.y - radius;
        stepDy = 64.0f;
        distY = std::max(0.0f, dy / norm.y);
    }
    else if (norm.y < 0)
    {
        // Moving backwards
        dy = cellY * 64.0f - g_position.y + radius;
        stepDy = -64.0f;
        distY = std::max(0.0f, dy / norm.y);
    }

    //
    // Core DDA loop. We pick the axis along which we'll intersect the next
    // grid boundary (which of distX or distY is closest), and then advance up
    // to the point of touching that wall. Then, we check if that's a collision,
    // if so, we update our movement trajectory and recurse. If it's not a collision,
    // we update our DDA parameters and advance another iteration. This continues until
    // we move a total distance equal to the movement vector length passed in.
    //
    XMFLOAT2 newPosition(position);
    for (;;)
    {
        int cellXOffset = 0;
        int cellYOffset = 0;

        if (distX <= distY)
        {
            distance = distX;
            dx += stepDx;
            distX = dx / norm.x;
            cellXOffset = norm.x > 0 ? 1 : -1;
        }
        else
        {
            distance = distY;
            dy += stepDy;
            distY = dy / norm.y;
            cellYOffset = norm.y > 0 ? 1 : -1;
        }

        if (distance > totalDistance)
        {
            // Reached end of movement, clamp in case we overshot, but use a value just shy
            // of the real distance to avoid getting stuck in the wall due to FP error.
            distance = totalDistance * OneMinusEpsilon;

            // Resolve position and we're done.
            newPosition.x = position.x + norm.x * distance;
            newPosition.y = position.y + norm.y * distance;

            return pfnReachedEnd(newPosition);
        }

        // Advance to the new position, update cellX & cellY,
        // and check if the bordering cell is empty or not.
        newPosition.x = position.x + norm.x * distance;
        newPosition.y = position.y + norm.y * distance;

        cellX = (int)newPosition.x / 64 + cellXOffset;
        cellY = (int)newPosition.y / 64 + cellYOffset;

        if (cellX < 0 || cellX > 63 || cellY < 0 || cellY > 63)
        {
            // Exited the bounds of the level! Can't collide with anything out there
            return -1;
        }

        if (!IsCellEmpty(cellX, cellY))
        {
            return pfnCollision(newPosition, cellX, cellY, movement, (OneMinusEpsilon - distance / totalDistance), cellXOffset != 0);
        }
    }
}

int HandleMoveWithCollision(const XMFLOAT2& movement)
{
    return RaycastDDA(g_position, PlayerRadius, movement,
        // What to do when we reach the end of movement without hitting anything
        // NOTE: since we recurse, this return value gets ignored in many cases.
        [](const XMFLOAT2& position)
        {
            g_position = position;
            return -1;
        },
        // What to do when we collide with something
        [](const XMFLOAT2& position, int cellX, int cellY, const XMFLOAT2& movement, float remainingDistance, bool xHit)
        {
            UNREFERENCED_PARAMETER(cellX);
            UNREFERENCED_PARAMETER(cellY);

            g_position = position;
            HandleMoveWithCollision(xHit ? XMFLOAT2(0, movement.y * remainingDistance) : XMFLOAT2(movement.x * remainingDistance, 0));
            return 1;
        });
}

int GetCellInFront(const XMFLOAT2& position, const XMFLOAT2& direction)
{
    return RaycastDDA(position, 1.0f, direction,
        // What to do when we reach the end of direction without hitting anything
        [](const XMFLOAT2& position)
        {
            UNREFERENCED_PARAMETER(position);
            return -1;
        },
        // What to do when we collide with something
        [](const XMFLOAT2& position, int cellX, int cellY, const XMFLOAT2& movement, float remainingDistance, bool xHit)
        {
            UNREFERENCED_PARAMETER(position); 
            UNREFERENCED_PARAMETER(movement); 
            UNREFERENCED_PARAMETER(remainingDistance); 
            UNREFERENCED_PARAMETER(xHit); 
            return cellY * 64 + cellX;
        });
}
