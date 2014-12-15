#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_ENGINE_INTERNAL_H_
#define _GDK_ENGINE_INTERNAL_H_

struct IStream;

namespace GDK
{
    struct IGameHost;
    struct IEngine;

    extern "C" HRESULT CreateEngine(_In_ IGameHost* host, _In_ IStream* configData, _Deref_out_ IEngine** engine);
}

#endif // _GDK_ENGINE_INTERNAL_H_
