#include "precomp.h"

static const wchar_t ClassName[] = L"Quake2";

static HWND InitializeAndCreateWindow(HINSTANCE instance, uint32_t width, uint32_t height);
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    HWND hwnd = nullptr;

    InputStartup();

    try
    {
        hwnd = InitializeAndCreateWindow(instance, 1280, 720);

        std::unique_ptr<Pak> pak(new Pak(L"C:\\GDK\\Main\\Games\\Quake2\\content\\Pak\\pak0.pak"));
        std::unique_ptr<Renderer> renderer(new Renderer(hwnd));

        uint32_t size;
        auto chunk = pak->GetChunk("maps/base1.bsp", &size);

        std::unique_ptr<Bsp> bsp(new Bsp(renderer->GetDevice(), pak, chunk));

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        XMFLOAT3 position(0, 0, 0);
        XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
        XMVECTOR right = XMVectorSet(1, 0, 0, 0);
        XMVECTOR up = XMVectorSet(0, 1, 0, 0);
        XMFLOAT4X4 cameraWorld, view, projection;

        XMMATRIX rot(right, up, forward, XMVectorSet(0, 0, 0, 1));
        XMStoreFloat4x4(&cameraWorld, XMMatrixAffineTransformation(XMVectorSet(1, 1, 1, 0), XMVectorZero(), XMQuaternionRotationMatrix(rot), XMLoadFloat3(&position)));
        XMStoreFloat4x4(&view, XMMatrixLookToLH(XMLoadFloat3(&position), forward, up));
        XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(60), 16.0f / 9.0f, 0.1f, 1000.0f));

        auto& input = GetInput();

        LARGE_INTEGER prev, now, freq;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&prev);
        wchar_t message[100];

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
                QueryPerformanceCounter(&now);
                float delta = (float)(((double)now.QuadPart - (double)prev.QuadPart) / (double)freq.QuadPart);
                swprintf_s(message, L"FPS: %3.2f\n", 1.0f / delta);
                OutputDebugString(message);

                prev = now;

                input.Tick();

                if (input.IsMenuDismissPressed())
                {
                    PostQuitMessage(0);
                }

                XMFLOAT2 movement = input.GetMovementVector();
                XMFLOAT2 look = input.GetLookVector();
                bool changed = false;

                static const float MoveSpeed = 200.0f;
                static const float LookSpeed = 3.0f;

                if (movement.x != 0 || movement.y != 0)
                {
                    XMVECTOR pos = XMLoadFloat3(&position);
                    pos = XMVectorAdd(pos, XMVectorAdd(XMVectorScale(right, movement.x * MoveSpeed * delta), XMVectorScale(forward, movement.y * MoveSpeed * delta)));
                    XMStoreFloat3(&position, pos);
                    changed = true;
                }

                if (look.x != 0 || look.y != 0)
                {
                    XMMATRIX rotation = XMMatrixMultiply(XMMatrixRotationAxis(right, -look.y * LookSpeed * delta), XMMatrixRotationAxis(up, look.x * LookSpeed * delta));
                    forward = XMVector3Transform(forward, rotation);
                    right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forward);
                    up = XMVector3Cross(forward, right);
                    changed = true;
                }

                if (changed)
                {
                    XMMATRIX rot(right, up, forward, XMVectorSet(0, 0, 0, 1));
                    XMStoreFloat4x4(&cameraWorld, XMMatrixAffineTransformation(XMVectorSet(1, 1, 1, 0), XMVectorZero(), XMQuaternionRotationMatrix(rot), XMLoadFloat3(&position)));
                    XMStoreFloat4x4(&view, XMMatrixLookToLH(XMLoadFloat3(&position), forward, up));
                }

                renderer->Clear();

                renderer->DrawBsp(cameraWorld, view, projection, bsp);

                renderer->Present();
            }
        }
    }
    catch (const std::exception&)
    {
    }

    if (hwnd != nullptr)
    {
        DestroyWindow(hwnd);
    }

    InputShutdown();

    return 0;
}

HWND InitializeAndCreateWindow(HINSTANCE instance, uint32_t width, uint32_t height)
{
    WNDCLASSEX wcx = {};
    wcx.cbSize = sizeof(wcx);
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcx.hInstance = instance;
    wcx.lpfnWndProc = WndProc;
    wcx.lpszClassName = ClassName;

    CHECKGLE(RegisterClassEx(&wcx));

    DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

    RECT size = {};
    size.right = width;
    size.bottom = height;
    AdjustWindowRect(&size, style, FALSE);

    HWND hwnd = CreateWindow(ClassName, ClassName, style, CW_USEDEFAULT, CW_USEDEFAULT,
        size.right - size.left, size.bottom - size.top, nullptr, nullptr, instance, nullptr);

    CHECKNOTNULL(hwnd);

    return hwnd;
}

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
