#pragma once

#include <GDKError.h>

#include <al.h>
#include <alc.h>

#define CHECK_AL(x) x; GDK::CheckALError(TOKENIZE_SOURCE(x), __FILE__, __LINE__);

// Macro to simply help with CheckGLError below. Not useful anywhere else
#define CHECKALERROR_CASE(x) case x: { char formattedSource[512] = {0}; sprintf_s(formattedSource, "%s (Error: %s)", source, #x); throw GDK::GDKException(formattedSource, filename, lineNumber, static_cast<int32_t>(alerr)); }

namespace GDK
{
    static inline void CheckALError(_In_opt_z_ const char* source, _In_z_ const char* filename, _In_ int32_t lineNumber)
    {
        ALenum alerr = alGetError(); 
        if (alerr != AL_NO_ERROR) 
        {
            switch (alerr)
            {
                CHECKALERROR_CASE(AL_INVALID_ENUM);
                CHECKALERROR_CASE(AL_INVALID_OPERATION);
                CHECKALERROR_CASE(AL_INVALID_VALUE);
                CHECKALERROR_CASE(AL_OUT_OF_MEMORY);
                CHECKALERROR_CASE(AL_INVALID_NAME);
                default: throw GDK::GDKException(source, filename, lineNumber, static_cast<int32_t>(alerr)); 
            }
        }
    }
}