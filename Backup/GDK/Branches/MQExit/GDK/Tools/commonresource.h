#pragma once

#include <Windows.h>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#pragma warning(pop)

#pragma pack(push,1)

struct COMMON_RESOURCE_FILEHEADER
{
    uint32_t Version;   // set to XXXX_RESOURCE_VERSION
    char Name[64];      // name of resource
    unsigned __int64 Id;
};

#pragma pack(pop)
