#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_ENGINE_H_
#define _GDK_ENGINE_H_

namespace GDK
{
    // forward declarations
    struct IEngineDebug;
    struct IEngineEdit;
    struct IMemoryStats;

    enum ExecutionMode
    {
        ExecutionMode_Normal,
        ExecutionMode_Edit,
    };

    // The main engine interface, which allows control of
    // a running instance of the game engine.
    struct IEngine : IRefCounted
    {
        // let's the engine know that the host is idle, which let's the engine run some code.
        // When the engine is configured for a fixed time step, this may not result in an actual frame
        // being run. If the engine is running as fast as possible, then this normally results in a frame.
        GDK_IMETHOD_(void) OnIdle() = 0;

        // Gets or sets the execution mode of the engine
        GDK_IMETHOD_(ExecutionMode) GetExecutionMode() const = 0;
        GDK_IMETHOD SetExecutionMode(_In_ ExecutionMode mode) = 0;

        // Get access to debug and editing capabilities
        GDK_IMETHOD_(stde::ref_ptr<IEngineDebug>)   GetDebugger() = 0;
        GDK_IMETHOD_(stde::ref_ptr<IEngineEdit>)    GetEditor() = 0;
        GDK_IMETHOD_(stde::ref_ptr<IMemoryStats>)   GetMemoryStats() = 0;
    };

    // Provides access to debug functionality within the engine
    struct IEngineDebug : IRefCounted
    {
    };

    // Provides access to editing capabilites within the engine. These methods will fail if the engine
    // is not in Edit execution mode.
    struct IEngineEdit : IRefCounted
    {
    };
}

#endif // _GDK_ENGINE_H_
