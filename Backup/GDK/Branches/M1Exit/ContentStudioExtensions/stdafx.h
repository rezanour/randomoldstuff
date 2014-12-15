#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

// Many of the includes use string functions that are reporting deprecated warnings.
// These warnings should be disabled since this code is not going to fix or modify
// them to no longer report this warning.  All new string operations will be performed
// using either MFC's CString, or STL's std::wstring.
#pragma warning( disable : 4996 ) // disable deprecated warning 
#pragma warning( disable : 4995 ) // disable deprecated warning 

#include <Windows.h>
#include <strsafe.h>
#include <string>
#include <vector>
#include "MemoryDC.h"

// ContentStudio SDK tools
#include <GDK\Tools\contentstudioproperties.h>
#include <GDK\Tools\textureresource.h>
#include <GDK\Tools\geometryresource.h>
#include <GDK\Tools\uiextension.h>
#include <GDK\Tools\dataextension.h>
#include <GDK\Tools\resourcefactory.h>

// Content Browser extension
#include "ContentBrowserWindow.h"
#include "ContentBrowserExtension.h"

// WaveFront Model extension
#include "WaveFrontModelExtension.h"

// Texture Resource extension
#include "TextureResourceExtension.h"

