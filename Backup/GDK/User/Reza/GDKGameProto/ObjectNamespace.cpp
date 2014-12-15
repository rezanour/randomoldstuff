#include "StdAfx.h"
#include "ObjectNamespace.h"

using GDK::ObjectNamespace;
using GDK::IObject;

ObjectNamespace::ObjectNamespace()
{
}

ObjectNamespace::~ObjectNamespace()
{
}

HRESULT ObjectNamespace::Create(ObjectNamespace** ppObjMgr)
{
    if (!ppObjMgr)
        return E_POINTER;

    *ppObjMgr = GDKNEW("ObjectNamespace") ObjectNamespace;
    return (*ppObjMgr) ? S_OK : E_OUTOFMEMORY;
}

HRESULT ObjectNamespace::AddObject(IObject* object)
{
    UNREFERENCED_PARAMETER(object);
    return E_NOTIMPL;
}

HRESULT ObjectNamespace::FindObjectByName(const char* name, IObject** ppObject)
{
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(ppObject);
    return E_NOTIMPL;
}
