#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

// resource handles
#define INVALID_RESOURCE    -1
typedef uint_t resource_t;

// resource types (16 types max since they are stored in half of a byte)
typedef enum
{
    resource_type_flat = 0,
    resource_type_sprite,
    resource_type_texture,
    resource_type_soundfx,
    resource_type_music,
    resource_type_map,
    resource_type_soundbuffer,
    resource_type_max,
} resource_type_t;

typedef void* resourcemanager_t;

// creating and managing resource managers
resourcemanager_t ResourceManagerCreate();
void ResourceManagerDestroy(resourcemanager_t resourceManager);

// get a resource, which will load it if it's not already loaded
resource_t ResourceManagerGet(resourcemanager_t resourceManager, resource_type_t type, const char* name);

void* ResourceManagerDataFromHandle(resource_type_t type, resource_t handle);

#endif // _RESOURCEMANAGER_H_