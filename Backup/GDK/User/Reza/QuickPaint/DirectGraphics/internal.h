#ifndef _INTERNAL_H_
#define _INTERNAL_H_

//
// common standard headers
//

#include <stdio.h>
#include <assert.h>
#include <memory.h>


//
// common types and definitions
//

#ifndef NULL
#define NULL 0
#endif

#ifndef min
#define min(a, b) (a) < (b) ? (a) : (b)
#endif

#ifndef max
#define max(a, b) (a) > (b) ? (a) : (b)
#endif


//
// graphics utility methods
//

bool_t DGIsValidBpp(byte_t bpp);


//
// memory manager initialize and cleanup
//

bool_t  DGInitializeMemoryManager();
void    DGUninitializeMemoryManager(bool_t reportMemoryLeaks);


//
// internal DirectGraphics headers
//

#include "internal_debug.h"


#endif // _INTERNAL_H_