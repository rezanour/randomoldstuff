#pragma once

#include <GDKError.h>

#ifdef WIN32
#include <GL/glew.h>
#include <GL/wglew.h>
#else
#error Fill in the right includes here for the platform.
#endif

#define CHECK_GL(x) x; GDK::CheckGLError(TOKENIZE_SOURCE(x), __FILE__, __LINE__);

// Macro to simply help with CheckGLError below. Not useful anywhere else
#define CHECKGLERROR_CASE(x) case x: { char formattedSource[512] = {0}; sprintf_s(formattedSource, "%s (Error: %s)", source, #x); throw GDK::GDKException(formattedSource, filename, lineNumber, static_cast<int32_t>(glerr)); }

namespace GDK
{
    static inline void CheckGLError(_In_opt_z_ const char* source, _In_z_ const char* filename, _In_ int32_t lineNumber)
    {
        GLenum glerr = glGetError(); 
        if (glerr != GL_NO_ERROR) 
        {
            switch (glerr)
            {
                CHECKGLERROR_CASE(GL_INVALID_ENUM);
                CHECKGLERROR_CASE(GL_INVALID_OPERATION);
                CHECKGLERROR_CASE(GL_INVALID_VALUE);
                CHECKGLERROR_CASE(GL_OUT_OF_MEMORY);
                CHECKGLERROR_CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
                CHECKGLERROR_CASE(GL_STACK_OVERFLOW);
                CHECKGLERROR_CASE(GL_STACK_UNDERFLOW);
                default: throw GDK::GDKException(source, filename, lineNumber, static_cast<int32_t>(glerr)); 
            }
        }
    }
}