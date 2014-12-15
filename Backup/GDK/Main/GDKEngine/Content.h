#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_CONTENT_H_
#define _GDK_CONTENT_H_

namespace GDK
{
    struct IContent;
    typedef stde::ref_ptr<IContent> ContentPtr;

    // Implemented by all asssets, resources, etc... 
    struct IContent : IRefCounted
    {
        GDK_IMETHOD_(uint64_t) GetContentId() const = 0;
        GDK_IMETHOD_(bool) IsLoaded() const = 0;
        GDK_IMETHOD_(void) Unload() = 0;
        GDK_IMETHOD_(void) Load() = 0;
    };
} // GDK

#endif // _GDK_CONTENT_H_
