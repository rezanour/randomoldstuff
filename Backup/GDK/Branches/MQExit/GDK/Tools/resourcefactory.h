#pragma once

#include <windows.h>
#include <GDK\Tools\textureresource.h>
#include <GDK\Tools\spritefontresource.h>
#include <GDK\Tools\geometryresource.h>

[uuid("4243b8f5-9fb7-4cbe-9e61-5456d60af73f")]
struct IContentStudioResourceFactory : public IUnknown
{
    virtual HRESULT CreateTextureResource(PCSTR name, ITextureResource** ppResource) = 0;
    virtual HRESULT CreateGeometryResource(PCSTR name, IGeometryResource** ppResource) = 0;
};
