#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_ENGINECORE_H_
#define _GDK_ENGINECORE_H_

////////////////////////////////////////////////////////////////////////////////
// core interface to the engine which is exposed to hosts

namespace GDK
{
    // foreward declarations
    struct IGDKHost;
    struct Message;

    struct IGDKMemoryTracker : IRefCounted
    {
        GDK_IMETHOD GetTotalReservedSystemMemory(_Out_ uint64_t* bytes) = 0;
        GDK_IMETHOD GetUsedSystemMemory(_Out_ uint64_t* bytes) = 0;
        GDK_IMETHOD GetNumberOfAllocations(_Out_ uint64_t* numAllocations) = 0;
    };

    struct IGDKDebugCallback : IRefCounted
    {
        GDK_IMETHOD_(void) OnWarning(_In_ const wchar_t* message) = 0;
        GDK_IMETHOD_(void) OnError(_In_ const wchar_t* message) = 0;
    };

    struct IGDKEngineCore : IRefCounted
    {
        // execution
        GDK_IMETHOD Tick() = 0;

        // get global statistics
        GDK_IMETHOD GetMemoryTracker(_Out_ IGDKMemoryTracker** memoryTracker) = 0;

        // configure debug and logging options
        GDK_IMETHOD RegisterDebugCallback(_In_ IGDKDebugCallback* callback) = 0;
        GDK_IMETHOD UnregisterDebugCallback(_In_ IGDKDebugCallback* callback) = 0;
        GDK_IMETHOD EnableLogging(_In_ bool enabled) = 0;
        GDK_IMETHOD SetLogFile(_In_ const wchar_t* filename) = 0;

        // send messages
        GDK_IMETHOD PostMessage(_In_ const Message& message) = 0;
    };

    HRESULT GDK_PUBLIC GDK_API CreateEngine(_In_ IGDKHost* host, _Out_ IGDKEngineCore** engine);
} // GDK

#endif // _GDK_ENGINECORE_H_
