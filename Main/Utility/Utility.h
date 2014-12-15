//==============================================================================
//
// Utility is a library of common types, algorithms, and other helpful utilities
// to be used by any application. The library makes every attempt to not impose any
// particular patterns on it's callers, and to be as flexible and useful as possible.
//
// Most of the interface is designed to be directly consumable by C and C++ programs.
// However, there are a small number of utilities that are only available for C++, and
// these are carefully separated so that the header is still consumable by C projects.
//
//==============================================================================
#pragma once

//==============================================================================
// C-compatible portion of the header, can be used by any project
//==============================================================================

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

typedef uint8_t byte_t;

#if !defined(__cplusplus)
typedef enum { false, true } bool;
#endif

//
// Lightweight, fast user-mode only locks.
//
typedef struct
{
    volatile long value;
} UMSpinLock;

void UMSpinLockInitialize(_Out_ UMSpinLock* lock);
bool UMSpinLockTryAcquire(_Inout_ UMSpinLock* lock, uint32_t spinCount);
void UMSpinLockAcquire(_Inout_ UMSpinLock* lock);
void UMSpinLockRelease(_Inout_ UMSpinLock* lock);

typedef struct
{
    UMSpinLock Lock;
    uint32_t CurrentOwner;
    uint32_t* ThreadWaitQueue;
    uint16_t Head;
    uint16_t Tail;
    uint16_t WaitQueueLength;
} UMCriticalSection;

void UMCriticalSectionInitialize(_Out_ UMCriticalSection* cs, uint16_t maxWaitChain);
void UMCriticalSectionDestroy(_Inout_ UMCriticalSection* cs);
void UMCriticalSectionAcquire(_Inout_ UMCriticalSection* cs);
void UMCriticalSectionRelease(_Inout_ UMCriticalSection* cs);

//
// Handle system.
//
// The binary-wide handle table supports up  to 2^18 (262144) handles
// to up to 2^8 - 1 (255) unique object types (0 is not allowed as a valid type)
//
typedef uint32_t Handle;
#define InvalidHandle 0

bool HandleTableStartup();
void HandleTableShutdown();
Handle AllocHandle(_In_ void* object, _In_ uint8_t type);
void FreeHandle(_In_ Handle handle);
void* GetObjectFromHandle(_In_ Handle handle);

//
// String conversions.
//
// char to wchar_t conversion and vice versa.
// NOTE: This is not a true codepage/charset conversion.
// It is useful for quickly expanding/compressing a basic char* to wchar_t* & back
//

bool ConvertCharToWChar(_In_z_ const char* source, _Out_writes_(destCount) wchar_t* dest, _In_ size_t destCount);
bool ConvertWCharToChar(_In_z_ const wchar_t* source, _Out_writes_(destCount) char* dest, _In_ size_t destCount);

//
// Simple axis-aligned bounding box tree
//

typedef struct _AabbNode
{
    float Mins[3];
    float Maxs[3];

    struct _AabbNode* Parent;

#pragma warning(push)
#pragma warning(disable: 4201) // nameless union
    union
    {
        void* Object;
        struct _AabbNode* Left;
    };
#pragma warning(pop)

    struct _AabbNode* Right;

    bool BoundsDirty;
} AabbNode;


typedef AabbNode* (*pfnAabbNodeAlloc)();
typedef void (*pfnAabbNodeFree)(_In_ AabbNode* node);
typedef void (*pfnAabbNodeGetBoundsFromObject)(_In_ void* object, _Out_writes_(3) float mins[3], _Out_writes_(3) float maxs[3]);

// This MUST be called to provide the bounds function by the client prior to using the AabbNode implementation
void AabbNodeStartup(_In_ pfnAabbNodeGetBoundsFromObject pfnGetBounds);

// Client may optionally override alloc/free (for example, to use a pool or array to satisfy requests)
void AabbNodeSetAllocFree(_In_ pfnAabbNodeAlloc pfnAlloc, _In_ pfnAabbNodeFree pfnFree);

bool AabbNodeIsLeaf(_In_ const AabbNode* node);
void AabbNodeMarkDirty(_In_ AabbNode* node);

void AabbNodeGetBounds(_In_ AabbNode* node, _Out_writes_(3) float mins[3], _Out_writes_(3) float maxs[3]);

AabbNode* AabbNodeInsert(_Inout_ AabbNode** root, _In_count_(3) const float mins[3], _In_count_(3) const float maxs[3], _In_ void* object);
void AabbNodeRemove(_Inout_ AabbNode** root, _In_ AabbNode* node);

#if defined(__cplusplus)
} // extern "C"
#endif


//==============================================================================
// C++ only utilities
//==============================================================================
#if defined(__cplusplus)

#endif
