#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_MEMORY_STATS_H_
#define _GDK_MEMORY_STATS_H_

namespace GDK
{
    // Components which want to report memory stats back to the host
    // implement this interface, which the host can query for.
    struct IMemoryStats : IRefCounted
    {
        // returns the total number of bytes reserved by the component
        GDK_IMETHOD_(size_t) GetTotalMemory() const = 0;

        // returns the total number of bytes in use by the component
        GDK_IMETHOD_(size_t) GetUsedMemory() const = 0;
    };
}

#endif // _GDK_MEMORY_STATS_H_
