#include "StdAfx.h"

extern "C" HRESULT GDK::CreateRenderer(_In_ HWND hwnd, _Deref_out_ GDK::IRenderer** renderer)
{
    assert(hwnd);
    assert(renderer);

    try
    {
        *renderer = Renderer::Create(hwnd).detach();
    }
    catch (const std::exception& ex)
    {
        LogFailure(ex.what());
        return E_FAIL;
    }

    return S_OK;
}
