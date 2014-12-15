#include "Precomp.h"
#include "Transform.h"
#include "GraphicsSystem.h"
#include "ScreenSystem.h"
#include "AudioSystem.h"
#include "Input.h"
#include "Contexts.h"
#include "Screen.h"
#include "Screens\TitleScreen.h"
#include "Screens\MainMenuScreen.h"
#include "Screens\GameplayScreen.h"
#include "Screens\GraphicsTestScreen.h"
#include "Screens\AITestScreen.h"
#include "Model.h"

static const wchar_t GameTitle[] = L"OldSchool";
static const uint32_t Width = 1280;
static const uint32_t Height = 800;

static HWND CreateGameWindow(_In_ HINSTANCE instance, _In_ uint32_t width, _In_ uint32_t height);
static LRESULT CALLBACK WndProc(_In_ HWND hwnd, _In_ uint32_t msg, _In_ WPARAM wParam, _In_ LPARAM lParam);

static float g_totalElapsedTime = 1.0f;
static float g_elapsedTime = 1.0f;
static BOOL g_firstUpdate = TRUE;
static LARGE_INTEGER g_performanceFrequency;
static LARGE_INTEGER g_startCounter;
static LARGE_INTEGER g_lastCounter;
static LARGE_INTEGER g_currentCounter;

void UpdateElapsedTime(_In_ UpdateContext* updateContext);

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, char* commandLine, int showCommand)
{
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(commandLine);

#if 0
    CollisionQuery query;

    query.WorldPosition = XMVectorSet(0, 0, 0, 1);
    query.WorldMovement = XMVectorSet(10, 0, 0, 0);

    query.eRadius = XMVectorSet(2, 2, 2, 0);

    // elipsoid space
    query.BasePoint = query.WorldPosition / query.eRadius;
    query.Velocity = query.WorldMovement / query.eRadius;
    query.VelocityLen2 = XMVector3LengthSq(query.Velocity);

    query.Collided = false;

    XMVECTOR v0 = XMVectorSet(3, 0, 1, 1);
    XMVECTOR v1 = XMVectorSet(3, 1, 0, 1);
    XMVECTOR v2 = XMVectorSet(3, 0, -1, 1);

    XMVECTOR v3 = XMVectorSet(2, -1, 1, 1);
    XMVECTOR v4 = XMVectorSet(3, 0, 0, 1);
    XMVECTOR v5 = XMVectorSet(2, -1, -1, 1);

    XMVECTOR v6 = XMVectorSet(0, 5, -1, 1);
    XMVECTOR v7 = XMVectorSet(3, 1, 0, 1);
    XMVECTOR v8 = XMVectorSet(0, 5, 1, 1);

    bool result = false;

    do
    {
        TestEllipsoidTriangle(query, v0 / query.eRadius, v1 / query.eRadius, v2 / query.eRadius);
        TestEllipsoidTriangle(query, v3 / query.eRadius, v4 / query.eRadius, v5 / query.eRadius);
        TestEllipsoidTriangle(query, v6 / query.eRadius, v7 / query.eRadius, v8 / query.eRadius);
        if (query.Collided)
        {
            XMFLOAT3 pt;
            XMStoreFloat3(&pt, query.CollisionPoint * query.eRadius);

            wchar_t message[1000] = {};
            StringCchPrintf(message, _countof(message), L"Dist: %3.2f, Pt: (%3.2f, %3.2f, %3.2f)\n", XMVectorGetX(XMVectorMultiply(query.CollisionDistance, query.eRadius)), pt.x, pt.y, pt.z);
            OutputDebugString(message);

            result = ResolveCollision(query);
            query.WorldPosition = query.BasePoint * query.eRadius;
            query.WorldMovement = query.Velocity * query.eRadius;

            XMFLOAT3 pos, vel;
            XMStoreFloat3(&pos, query.WorldPosition);
            XMStoreFloat3(&vel, query.WorldMovement);

            StringCchPrintf(message, _countof(message), L"Keep going? %s, Pos: (%3.2f, %3.2f, %3.2f), Vel: (%3.2f, %3.2f, %3.2f)\n", result ? L"no" : L"yes", pos.x, pos.y, pos.z, vel.x, vel.y, vel.z);
            OutputDebugString(message);
        }
    } while (!result);

#endif

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        return hr;
    }

    // Destroy all com pointers before CoUninit
    {
        HWND hwnd = CreateGameWindow(instance, Width, Height);
        MSG msg = {};

        GraphicsSystemConfig config = {};
        config.Window = hwnd;
        config.Width = Width;
        config.Height = Height;
        config.VSync = true;

        auto graphics = CreateGraphicsSystem(config);
        auto screenSystem = CreateScreenSystem();
        auto input = CreateInput();
        auto audioSystem = CreateAudioSystem();

        UpdateWindow(hwnd);
        ShowWindow(hwnd, showCommand);

        UpdateContext updateContext;
        DrawContext drawContext;

        updateContext.ElapsedTime = 0.0f;
        updateContext.GraphicsSystem = graphics;
        updateContext.Input = input;
        updateContext.AudioSystem = audioSystem;
        updateContext.ScreenSystem = screenSystem;

        drawContext.GraphicsSystem = graphics;

        screenSystem->PushScreen(TitleScreen::Create(graphics));

        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                Profiler::StartFrame();

                {
                    // Update
                    ProfileMark profileUpdate(CodeTag::Update);

                    // Update Input
                    input->Update();

                    if (input->IsKeyJustPressed('V'))
                    {
                        static bool enable = true;
                        graphics->EnableStereoRendering(enable);
                        enable = !enable;
                    }
                    else if (input->IsKeyJustPressed('P'))
                    {
                        static bool enable = true;
                        Profiler::EnableProfiler(enable);
                        enable = !enable;
                    }

                    // Update Audio
                    audioSystem->Update();

                    // Update game
                    UpdateElapsedTime(&updateContext);
                    screenSystem->Update(updateContext);
                }

                {
                    // Draw
                    ProfileMark profileDraw(CodeTag::Draw);

                    screenSystem->Draw(drawContext);

                    graphics->RenderFrame();
                }

                Profiler::EndFrame(graphics);
            }
        }
    }

    CoUninitialize();

    return 0;
}

_Use_decl_annotations_
HWND CreateGameWindow(HINSTANCE instance, uint32_t width, uint32_t height)
{
    WNDCLASSEX wcx = {};
    wcx.cbSize = sizeof(wcx);
    wcx.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wcx.hInstance = instance;
    wcx.lpfnWndProc = WndProc;
    wcx.lpszClassName = GameTitle;

    if (RegisterClassEx(&wcx) == 0)
    {
        throw std::exception();
    }

    HWND hwnd = CreateWindow(GameTitle, GameTitle, WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_POPUPWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, instance, nullptr);

    if (hwnd == nullptr)
    {
        throw std::exception();
    }

    return hwnd;
}

_Use_decl_annotations_
LRESULT CALLBACK WndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0; // handled
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

_Use_decl_annotations_
void UpdateElapsedTime(UpdateContext* updateContext)
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

    updateContext->ElapsedTime = g_elapsedTime;
}
