#include "StdAfx.h"
#include "Debug.h"

namespace CoreServices
{

    uint64 CreateUniqueId()
    {
        uint64 uniqueId = 0;
    
        GUID guid;
        if (FAILED(CoCreateGuid(&guid)))
        {
            DebugError("Failed to create GUID!");
            Assert(false);
        }

        uniqueId = (uint64)guid.Data1 << 32;
        uniqueId |= (uint64)guid.Data2 << 16;
        uniqueId |= guid.Data3;

        return uniqueId;
    }

};