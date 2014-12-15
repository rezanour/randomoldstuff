#pragma once

#include <Platform.h>
#include <GDKError.h>
#include <GDKMath.h>
#include <RuntimeObject.h>
#include <GDKStream.h>
#include <Resources.h>
#include <Graphics.h>

#include <memory>
#include <vector>
#include <string>

#if defined(WIN32)

//#include <gl\GL.h>
#   include <gl\glew.h>
#   include <gl\wglew.h>

#elif defined(__APPLE__)

#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>

// Defines because Apple's API uses vendor suffixes in some places
#   define glGenVertexArrays glGenVertexArraysAPPLE
#   define glBindVertexArray glBindVertexArrayAPPLE
#   define glDeleteVertexArrays glDeleteVertexArraysAPPLE

#else
#   error Invalid platform for compilation.
#endif

#define CHECK_GL(x) { (x); GLenum ret = glGetError(); if (ret != GL_NO_ERROR) { throw GDK::Exception(ret, TXT(x), __FILEW__, __LINE__); } }

#include "OpenGLGraphicsDevice.h"
#include "RuntimeGeometry.h"
#include "RuntimeTexture.h"
