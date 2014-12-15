#ifndef _OBJECT_H_
#define _OBJECT_H_

typedef void* object_t;

#define INVALID_OBJECT  (object_t)-1

#define OBJECT_EMPTY    0
#define OBJECT_IMAGE    1
#define OBJECT_MAX      2

typedef void (*pfnDeleter)(void* p);

//
// create object
//

object_t QPCreateObject(byte_t type, void* data, pfnDeleter deleter);

//
// ref counting
//

void QPAddRefObject(object_t handle);
void QPReleaseObject(object_t* handle);

//
// data access
//

byte_t QPGetObjectType(object_t handle);
void* QPDataFromObject(byte_t type, object_t handle);


#endif // _OBJECT_H_