#pragma once

#pragma warning(disable : 4244) // '=' : conversion from 'wchar_t' to 'char', possible loss of data

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#include "afxwinappex.h"
#include "afxdialogex.h"

void __cdecl odprintf(const char *format, ...);

// Many of the includes use string functions that are reporting deprecated warnings.
// These warnings should be disabled since this code is not going to fix or modify
// them to no longer report this warning.  All new string operations will be performed
// using either MFC's CString, or STL's std::wstring.
#pragma warning( disable : 4996 ) // disable deprecated warning 
#pragma warning( disable : 4995 ) // disable deprecated warning 
#include <propkey.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <GdiPlus.h>
#include <math.h>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <fstream>
#include <map>
#include <memory>
#include <algorithm>
#include <xmllite.h>
#include <MMSystem.h>
#include <xnamath.h>
#include <DXUT.h>
#include <SDKmisc.h>
#include <DXUTgui.h>
#include <SDKMesh.h>
#include <DXUTCamera.h>

#include <System\Lucid3D.System.h>
//#undef DEBUG_PRINT

#include "Macros.h"
#include "stringutils.h"
#include "BufferObject.h"
#include "FileStream.h"
#include "MemoryStream.h"
#include "XmlFile.h"
#include "ProjectFile.h"

#include <DirectXTex.h>

#include "Resource.h"
#include "ViewTree.h"
#include "SolutionPane.h"
#include "OutputPane.h"
#include "PropertiesPane.h"

#include "ContentPlugin.h"
#include "TextureContent.h"
#include "TextureContentImpl.h"
#include "ModelContent.h"
#include "ModelContentImpl.h"
#include "SpriteFontContent.h"
#include "SpriteFontContentImpl.h"
#include "ContentContainer.h"
#include "ContentContainerImpl.h"
#include "ContentPluginServices.h"
#include "ContentPluginServicesImpl.h"
#include "ContentPluginAccess.h"
#include "ContentPluginManager.h"

#include "Lucid3DRenderer.h"
#include "MainFrm.h"
#include "ContentStudio.h"
#include "ContentStudioDoc.h"
#include "Direct3DView.h"
#include "ContentView.h"
#include "NullContentView.h"
#include "TextureContentView.h"
#include "ModelContentView.h"
#include "SpriteFontContentView.h"
#include "ContentStudioView.h"

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
