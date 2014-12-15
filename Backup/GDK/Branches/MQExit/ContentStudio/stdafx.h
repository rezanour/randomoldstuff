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

// Many of the includes use string functions that are reporting deprecated warnings.
// These warnings should be disabled since this code is not going to fix or modify
// them to no longer report this warning.  All new string operations will be performed
// using either MFC's CString, or STL's std::wstring.
#pragma warning( disable : 4996 ) // disable deprecated warning 
#pragma warning( disable : 4995 ) // disable deprecated warning 
#pragma warning( disable : 4005 ) // disable macro redefinition warning (intsafe.h)
#include <propkey.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <GdiPlus.h>
#include <math.h>
#include <xmllite.h>
#include <msxml6.h>
#include <MMSystem.h>
#include <DirectXMath.h>

// STL
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <fstream>
#include <map>
#include <memory>
#include <algorithm>

// Extended STL
#include <stde\types.h>
#include <stde\com_ptr.h>
#include <stde\conversion.h>

// Core Services
#include <CoreServices\FileStream.h>
#include <CoreServices\MemoryStream.h>
#include <CoreServices\Configuration.h>
#include <CoreServices\UniqueId.h>
#include <CoreServices\ContentTag.h>
#include <CoreServices\Debug.h>

// DirectX SDK Framework
#include "DXUT.h"
#include "SDKmisc.h"
#include "DXUTCamera.h"

// DirectX SDK Texture Library
#include <DirectXTex.h>

// DirectX SDK XNA Library
#include <SpriteBatch.h>
#include <GeometricPrimitive.h>
#include <CommonStates.h>

// General utilties
#include "Macros.h"
#include "stringutils.h"
#include "MemoryDC.h"

// Game Development Kit
#include <GDK\ContentTypes.h>

// Game Development Kit Tools
#include <GDK\Tools\textureresource.h>
#include <GDK\Tools\spritefontresource.h>
#include <GDK\Tools\geometryresource.h>
#include <GDK\Tools\contentstudioproperties.h>
#include <GDK\Tools\resourcefactory.h>
#include <GDK\Tools\uiextension.h>
#include <GDK\Tools\dataextension.h>
#include <GDK\Tools\commonproperties.h>

// ContentStudio
#include "Resource.h"
#include "xmlfile.h"
#include "PropertyEngine.h"
#include "Selector3D.h"
#include "ContentStudioPropertiesImpl.h"
#include "ContentStudioPropertiesCollectionImpl.h"
#include "ProjectBrowserExtension.h"
#include "ObjectPropertiesExtension.h"
#include "DirectXTextureResource.h"
#include "DirectXTextureResourceImpl.h"
#include "SpriteFontResourceImpl.h"
#include "GeometryResourceImpl.h"
#include "ResourceFactoryImpl.h"
#include "ResourceServices.h"
#include "ExtensionsManager.h"
#include "ContentStudioDocument.h"
#include "DockingWindowHost.h"
#include "MainFrm.h"
#include "ContentStudio.h"
#include "ContentStudioDoc.h"
#include "Direct3DView.h"
#include "ContentView.h"
#include "NullContentView.h"
#include "ContentStudio3DView.h"
#include "ContentStudioView.h"
#include "ContentStudioDocumentNotications.h"
#include "ImportTextureDlg.h"
#include "ExpressImportDlg.h"

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
