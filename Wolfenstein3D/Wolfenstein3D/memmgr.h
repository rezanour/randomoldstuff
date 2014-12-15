#ifndef _MEMMGR_H_
#define _MEMMGR_H_

#define RESOURCE_TYPE_NONE      0
#define RESOURCE_TYPE_SPRITE    1
#define RESOURCE_TYPE_SFX       2
#define RESOURCE_TYPE_MUSIC     3
#define RESOURCE_TYPE_MAP       4
#define RESOURCE_TYPE_MAX       5

typedef UINT    HALLOC;

#define INVALID_HALLOC_VALUE    0

/* allocate and free */
HALLOC MemAlloc(BYTE resourceType, UINT numBytes);
VOID MemFree(HALLOC h);

/* validate handle */
BOOL MemIsResource(HALLOC h, BYTE resourceType);

/* access the bits */
PVOID MemLock(HALLOC h);
VOID MemUnlock(HALLOC h);

/* stats */
BOOL MemIsInUse(HALLOC h);
UINT MemBytesUsedBy(BYTE resourceType);
UINT MemTotalBytesUsed();

#endif // _MEMMGR_H_