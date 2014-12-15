#ifndef _DIRECTGRAPHICS_H_
#define _DIRECTGRAPHICS_H_

//
// common types
//

#ifdef __cplusplus

typedef bool bool_t;

#else

typedef enum
{
    false = 0,
    true
} bool_t;

#endif

typedef unsigned char byte_t;
typedef unsigned short ushort_t;
typedef unsigned int uint_t;
typedef unsigned long ulong_t;
typedef unsigned long long uint64_t;


//
// system diagnostics
//

typedef void (__stdcall *pfnDebugOut)(const char* message);

void DGSetDebugOut(pfnDebugOut debugOut);


//
// initialization and cleanup of direct graphics system
//

bool_t DGInitialize();
void DGUninitialize(bool_t reportMemoryLeaks);

//
// DirectGraphics library
//

#include "dgmemory.h"
#include "dgsurface.h"
#include "dgdraw.h"

#endif // _DIRECTGRAPHICS_H_