#ifndef _DGMEMORY_H_
#define _DGMEMORY_H_

//
// memory allocation and release
//

void* DGAlloc(const char* const tag, uint_t numBytes);
void DGFree(void* p);


#endif // _DGMEMORY_H_