#include "StdAfx.h"

using namespace GDK;

GDK_METHOD GDKEngine::Create(_In_ IGDKHost* host, _Out_ GDKEngine** engine)
{
    HRESULT hr = S_OK;

    CHECKP(host, E_INVALIDARG);
    CHECKP(engine, E_POINTER);

    *engine = new GDKEngine(host);

Exit:
    return hr;
}

GDKEngine::GDKEngine(_In_ IGDKHost* host)
    : _host(host)
{
    assert(host);
    _host->AddRef();
}

GDKEngine::~GDKEngine()
{
    SafeRelease(_host);
}

GDK_IMETHODIMP GDKEngine::Tick()
{
    return S_OK;
}

GDK_IMETHODIMP GDKEngine::GetMemoryTracker(_Out_ IGDKMemoryTracker** memoryTracker)
{
    HRESULT hr = E_NOTIMPL;
    CHECKP(memoryTracker, E_POINTER);
    *memoryTracker = nullptr;

Exit:
    return hr;
}

GDK_IMETHODIMP GDKEngine::RegisterDebugCallback(_In_ IGDKDebugCallback* callback)
{
    HRESULT hr = E_NOTIMPL;
    CHECKP(callback, E_INVALIDARG);

Exit:
    return hr;
}

GDK_IMETHODIMP GDKEngine::UnregisterDebugCallback(_In_ IGDKDebugCallback* callback)
{
    HRESULT hr = E_NOTIMPL;
    CHECKP(callback, E_INVALIDARG);

Exit:
    return hr;
}

GDK_IMETHODIMP GDKEngine::EnableLogging(_In_ bool enabled)
{
    UNREFERENCED_PARAMETER(enabled);
    return E_NOTIMPL;
}

GDK_IMETHODIMP GDKEngine::SetLogFile(_In_ const wchar_t* filename)
{
    UNREFERENCED_PARAMETER(filename);
    return E_NOTIMPL;
}

GDK_IMETHODIMP GDKEngine::PostMessage(_In_ const Message& message)
{
    UNREFERENCED_PARAMETER(message);
    return S_OK;
}

GDK_METHOD GDK::CreateEngine(_In_ GDK::IGDKHost* host, _Out_ GDK::IGDKEngineCore** engine)
{
    HRESULT hr = S_OK;

    CHECKP(host, E_INVALIDARG);
    CHECKP(engine, E_POINTER);

    GDKEngine* theEngine;

    CHECK(GDKEngine::Create(host, &theEngine));

    *engine = theEngine;

Exit:
    return hr;
}
