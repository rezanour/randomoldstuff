#include "common.h"

typedef struct
{
    byte_t type;
    short refCount;
    void* data;
    pfnDeleter deleter;
} objectp_t;


//
// public object methods
//

object_t QPCreateObject(byte_t type, void* data, pfnDeleter deleter)
{
    objectp_t* object;

    if (type >= OBJECT_MAX)
    {
        DebugOut(L"Invalid object type specified: %d.\n", type);
        return INVALID_OBJECT;
    }

    object = (objectp_t*)DGAlloc("object", sizeof(objectp_t));
    if (!object)
    {
        DebugOut(L"Failed to allocate object.\n");
        return INVALID_OBJECT;
    }

    object->type = type;
    object->refCount = 1;
    object->data = data;
    object->deleter = deleter;

    return (object_t)object;
}

void QPAddRefObject(object_t handle)
{
    assert(handle && handle != INVALID_OBJECT);

    if (handle && handle != INVALID_OBJECT)
    {
        objectp_t* object = (objectp_t*)handle;
        ++object->refCount;
    }
}

void QPReleaseObject(object_t* handle)
{
    assert(handle && *handle && *handle != INVALID_OBJECT);

    if (handle && *handle && *handle != INVALID_OBJECT)
    {
        objectp_t* object = (objectp_t*)handle;

        if (--object->refCount == 0)
        {
            if (object->deleter)
            {
                object->deleter(object->data);
            }

            DGFree(object);
        }
    }

    *handle = INVALID_OBJECT;
}

byte_t QPGetObjectType(object_t handle)
{
    assert(handle && handle != INVALID_OBJECT);

    if (handle && handle != INVALID_OBJECT)
    {
        objectp_t* object = (objectp_t*)handle;
        return object->type;
    }

    return OBJECT_EMPTY;
}

void* QPDataFromObject(byte_t type, object_t handle)
{
    assert(handle && handle != INVALID_OBJECT);

    if (handle && handle != INVALID_OBJECT)
    {
        objectp_t* object = (objectp_t*)handle;

        assert(object->type == type);
        return (object->type == type) ? object->data : NULL;
    }

    return NULL;
}
