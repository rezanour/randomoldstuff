#include "precomp.h"
#include "input.h"

//#define USE_SOFTWARE

static HRESULT InitializeWindow(_In_ HINSTANCE instance, uint32_t width, uint32_t height, _Out_ HWND* hwnd);
static LRESULT CALLBACK WndProc(_In_ HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static HRESULT LoadFile(_In_z_ const wchar_t* filename, _Out_ std::unique_ptr<uint8_t[]>* buffer, _Out_ size_t* size);

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    HWND hwnd = nullptr;
    HRESULT hr = InitializeWindow(instance, 1280, 720, &hwnd);
    if (SUCCEEDED(hr))
    {
        InputStartup();

        Renderer* r;
        hr = Renderer::Create(hwnd, L"C:\\GDK\\Backup\\GDK\\Main\\Wolf3DContent\\raw", &r);
        if (SUCCEEDED(hr))
        {
            std::unique_ptr<Renderer> renderer(r);

            IWolf3DRenderer* w3dr;
#if defined(USE_SOFTWARE)
            uint32_t width = 640;
            uint32_t height = 480;
            std::unique_ptr<uint32_t[]> surface(new uint32_t[width * height]);

            ComPtr<ID3D11DeviceContext> context;
            renderer->GetDevice()->GetImmediateContext(&context);

            ComPtr<ID3D11Resource> renderTarget;

            hr = CreateWolf3DRendererSoftware(width, height, surface.get(), &w3dr);
#else
            hr = CreateWolf3DRendererDx(renderer->GetDevice().Get(), renderer->GetRenderTarget().Get(), &w3dr);
#endif
            if (SUCCEEDED(hr))
            {
                std::unique_ptr<IWolf3DRenderer> wolf3DRenderer(w3dr);
                std::unique_ptr<uint8_t[]> vswapData;
                size_t vswapSize;
                hr = LoadFile(L"C:\\GDK\\Backup\\GDK\\Main\\Wolf3DContent\\raw\\vswap.wl6", &vswapData, &vswapSize);
                if (FAILED(hr))
                {
                    return hr;
                }

                hr = wolf3DRenderer->LoadTextures(vswapData.get(), (uint32_t)vswapSize);
                if (FAILED(hr))
                {
                    return hr;
                }

                uint32_t currentLevel = 0;
                auto& level = renderer->GetLevel(currentLevel);
                wolf3DRenderer->SetWallLayer(level.Width, level.Height, level.Data.data());

                ShowWindow(hwnd, SW_SHOW);
                UpdateWindow(hwnd);

                static const float MoveSpeed = 0.1f;
                static const float TurnSpeed = 0.05f;
                XMVECTOR position = XMVectorSet(0, 0, -30, 0);
                XMVECTOR forward = XMVectorSet(1, 0, 0, 1);
                XMVECTOR up = XMVectorSet(0, 1, 0, 1);
                XMFLOAT4X4 view;
                XMFLOAT4X4 projection;

                RECT viewport = {};
                viewport.left = 0;
                viewport.top = 0;
                viewport.right = 1280;
                viewport.bottom = 720;

                auto& input = GetInput();

                renderer->ChangeLevel(0);

                XMStoreFloat4x4(&view, XMMatrixLookToLH(position, forward, up));
                XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(60), (float)(viewport.right - viewport.left)/ (viewport.bottom - viewport.top), 0.1f, 1000.0f));

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

                        if (input.IsMenuDismissPressed())
                        {
                            break;
                        }

                        if (input.IsMenuSelectPressed())
                        {
                            currentLevel = (currentLevel + 1) % renderer->GetNumLevels();
                            auto& nextLevel = renderer->GetLevel(currentLevel);
                            wolf3DRenderer->SetWallLayer(nextLevel.Width, nextLevel.Height, nextLevel.Data.data());
                        }

                        XMFLOAT2 movement = input.GetMovementVector();
                        XMFLOAT2 look = input.GetLookVector();
                        bool dirty = false;

                        if (movement.x != 0 || movement.y != 0)
                        {
                            XMVECTOR right = XMVector3Cross(up, forward);
                            position += XMVector3Normalize(right * movement.x + forward * movement.y) * MoveSpeed;
                            dirty = true;
                        }
                        if (look.x != 0)
                        {
                            XMMATRIX rotate = XMMatrixRotationY(look.x * TurnSpeed);
                            forward = XMVector3Normalize(XMVector3Transform(forward, rotate));
                            dirty = true;
                        }
                        if (dirty)
                        {
                            XMStoreFloat4x4(&view, XMMatrixLookToLH(position, forward, up));
                        }

                        wolf3DRenderer->DrawSprite3D(XMFLOAT2(0, -30), 96);
                        wolf3DRenderer->DrawSprite3D(XMFLOAT2(2, -30), 97);
                        wolf3DRenderer->DrawSprite3D(XMFLOAT2(4, -30), 95);

                        wolf3DRenderer->Render(view, projection, viewport);
#if defined(USE_SOFTWARE)
                        D3D11_BOX box = {};
                        box.left = viewport.left;
                        box.top = viewport.top;
                        box.right = box.left + width;
                        box.bottom = box.top + height;
                        box.back = 1;

                        renderer->GetRenderTarget()->GetResource(&renderTarget);
                        context->UpdateSubresource(renderTarget.Get(), 0, &box, surface.get(), width * sizeof(uint32_t), width * height * sizeof(uint32_t));
#endif
                        //renderer->Clear();
                        //renderer->Draw3DView(view, projection);

                        renderer->Present();
                    }
                }
            }
        }

        InputShutdown();

        DestroyWindow(hwnd);
    }

    return hr;
}

_Use_decl_annotations_
HRESULT InitializeWindow(HINSTANCE instance, uint32_t width, uint32_t height, HWND* hwnd)
{
    static const wchar_t ClassName[] = L"Wolfenstein 3D One Draw";

    assert(hwnd);

    *hwnd = nullptr;

    WNDCLASSEX wcx = {};
    wcx.cbSize = sizeof(wcx);
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcx.hInstance = instance;
    wcx.lpfnWndProc = WndProc;
    wcx.lpszClassName = ClassName;

    if (!RegisterClassEx(&wcx))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    RECT rc = {};
    rc.right = width;
    rc.bottom = height;
    AdjustWindowRect(&rc, style, FALSE);

    *hwnd = CreateWindow(ClassName, ClassName, style, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, instance, nullptr);

    if (!(*hwnd))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
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

_Use_decl_annotations_
HRESULT LoadFile(const wchar_t* filename, std::unique_ptr<uint8_t[]>* buffer, size_t* size)
{
    assert(buffer);
    assert(size);

    *size = 0;
    buffer->reset();

    FileHandle file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.IsValid())
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    DWORD fileSize = GetFileSize(file.Get(), nullptr);
    std::unique_ptr<uint8_t[]> data(new (std::nothrow) uint8_t[fileSize]);
    if (!data)
    {
        return E_OUTOFMEMORY;
    }

    DWORD read;
    if (!ReadFile(file.Get(), data.get(), fileSize, &read, nullptr))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    *size = (size_t)fileSize;
    buffer->swap(data);

    return S_OK;
}
