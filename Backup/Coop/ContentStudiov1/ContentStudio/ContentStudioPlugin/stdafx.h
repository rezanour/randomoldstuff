#pragma once

#pragma warning(disable : 4267) // 'argument' : conversion from 'something' to 'something else', possible loss of data
#pragma warning(disable : 4197) // export 'something' specified multiple times; using first specification
#pragma warning(disable : 4244) // '=' : conversion from 'wchar_t' to 'char', possible loss of data

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ObjIdl.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <xnamath.h>
#include <ContentPlugin.h>
#include "Common.h"
#include "PrimitiveFile.h"
#include "MCFile.h"
#include "ObjFile.h"
#include "3DWFile.h"
#include "SdkMeshFile.h"
