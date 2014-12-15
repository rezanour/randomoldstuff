#include "StdAfx.h"

using GDK::EngineCore;
using GDK::EngineCorePtr;

EngineCorePtr GDK_API EngineCore::Create(_In_ GDK::IGameHost* host)
{
    assert(host);

    EngineCorePtr engine;
    engine.attach(new EngineCore(host));

    engine->_sceneManager = new SceneManager;

    return engine;
}

EngineCore::EngineCore(_In_ GDK::IGameHost* host)
    : _host(host), _renderer(nullptr), _sceneManager(nullptr)
{
    assert(host && "Can't create an instance of the engine without a host");
    _host->AddRef();
}

EngineCore::~EngineCore()
{
    SafeDelete(_sceneManager);
    SafeRelease(_renderer);
    SafeRelease(_host);
}

GDK_IMETHODIMP_(void) EngineCore::OnIdle()
{
    _sceneManager->Step();
}

GDK_IMETHODIMP_(GDK::ExecutionMode) EngineCore::GetExecutionMode() const
{
    return ExecutionMode_Normal;
}

GDK_IMETHODIMP EngineCore::SetExecutionMode(_In_ GDK::ExecutionMode mode)
{
    UNREFERENCED_PARAMETER(mode);
    return S_OK;
}

GDK_IMETHODIMP_(stde::ref_ptr<GDK::IEngineDebug>) EngineCore::GetDebugger()
{
    stde::ref_ptr<IEngineDebug> debugger;
    debugger.reset(dynamic_cast<IEngineDebug*>(this));
    return debugger;
}

GDK_IMETHODIMP_(stde::ref_ptr<GDK::IEngineEdit>) EngineCore::GetEditor()
{
    stde::ref_ptr<IEngineEdit> editor;
    editor.reset(dynamic_cast<IEngineEdit*>(this));
    return editor;
}

GDK_IMETHODIMP_(stde::ref_ptr<GDK::IMemoryStats>) EngineCore::GetMemoryStats()
{
    stde::ref_ptr<IMemoryStats> stats;
    stats.reset(dynamic_cast<IMemoryStats*>(this));
    return stats;
}

GDK_IMETHODIMP_(void) EngineCore::AddRenderer(_In_ GDK::IRenderer* renderer)
{
    assert(renderer);

    // TODO: support multiple renderers (which can be exposed via options menu, etc...)
    if (!_renderer)
    {
        _renderer = renderer;
        _renderer->AddRef();
    }
}
