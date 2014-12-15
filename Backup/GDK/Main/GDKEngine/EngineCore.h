#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_ENGINECORE_H_
#define _GDK_ENGINECORE_H_

namespace GDK
{
    struct IRenderer;
    class SceneManager;

    class EngineCore;
    typedef stde::ref_ptr<EngineCore> EngineCorePtr;

    class EngineCore : private stde::non_copyable, public RefCounted<IEngine, IEngineDebug, IEngineEdit>
    {
    public:
        static EngineCorePtr GDK_API Create(_In_ IGameHost* host);

        // IEngine
        GDK_IMETHOD_(void) OnIdle();
        GDK_IMETHOD_(ExecutionMode) GetExecutionMode() const;
        GDK_IMETHOD SetExecutionMode(_In_ ExecutionMode mode);
        GDK_IMETHOD_(stde::ref_ptr<IEngineDebug>) GetDebugger();
        GDK_IMETHOD_(stde::ref_ptr<IEngineEdit>) GetEditor();
        GDK_IMETHOD_(stde::ref_ptr<IMemoryStats>) GetMemoryStats();

        // IEngineDebug

        // IEngineEdit

        // IGDKEngine
        GDK_IMETHOD_(void) AddRenderer(_In_ IRenderer* renderer);

    private:
        EngineCore(_In_ IGameHost* host);
        ~EngineCore();

        IGameHost* _host;
        IRenderer* _renderer;
        SceneManager* _sceneManager;
    };

} // GDK

#endif // _GDK_ENGINECORE_H_
