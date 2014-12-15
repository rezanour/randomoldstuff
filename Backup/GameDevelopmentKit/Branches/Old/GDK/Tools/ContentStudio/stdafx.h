#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#include <afxwin.h>
#include <afxext.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif

#include <propkey.h>

#include <afxcontrolbars.h>
#include "afxwinappex.h"
#include "afxdialogex.h"

#include <vector>
#include <string>
#include <map>
#include "stlutils.h"

// Allow GDK Math to interop with DirectXMath
#include <DirectXMath.h>
#define DIRECTXMATH_INTEROP

#include <Platform.h>
#include <ComponentFactory.h>
#include <GDKError.h>
#include <GDKMath.h>
#include <GDKString.h>
#include <GDKStream.h>
#include <RuntimeObject.h>
#include <Resources.h>
#include <Graphics.h>
#include <Game.h>
#include <Content.h>

using Microsoft::WRL::ComPtr;
using namespace GDK;
using namespace GDK::Graphics;

#include "Macros.h"
#include "Input.h"
#include "MemoryDC.h"
#include "Resource.h"
#include "INotifyView.h"
#include "TreeControl.h"
#include "Camera.h"
#include "RenderingPane.h"
#include "ArchiveExplorerContentPreviewPane.h"
#include "ArchiveExplorerPane.h"
#include "OutputPane.h"
#include "PropertiesPane.h"
#include "mainfrm.h"
#include "ContentStudio.h"
#include "ContentStudioDoc.h"
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
