#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_ENGINE_H_
#define _GDK_ENGINE_H_

////////////////////////////////////////////////////////////////////////////////
// core engine implementation

namespace GDK
{
    enum PlatformType;
    struct IGDKHost;
    struct IGDKEngineCore;

    class GDKEngine : public RefCounted<IGDKEngineCore>
    {
    public:
        // Creation
        static GDK_METHOD Create(_In_ IGDKHost* host, _Out_ GDKEngine** engine);

        // IGDKEngineCore
        GDK_IMETHOD Tick();
        GDK_IMETHOD GetMemoryTracker(_Out_ IGDKMemoryTracker** memoryTracker);
        GDK_IMETHOD RegisterDebugCallback(_In_ IGDKDebugCallback* callback);
        GDK_IMETHOD UnregisterDebugCallback(_In_ IGDKDebugCallback* callback);
        GDK_IMETHOD EnableLogging(_In_ bool enabled);
        GDK_IMETHOD SetLogFile(_In_ const wchar_t* filename);
        GDK_IMETHOD PostMessage(_In_ const Message& message);

    private:
        GDKEngine(_In_ IGDKHost* host);
        ~GDKEngine();

        IGDKHost* _host;
    };

} // GDK

#endif // _GDK_ENGINE_H_
