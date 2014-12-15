#include "Precomp.h"

using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Content {

ResourceFactory::ResourceFactory()
{

}

ResourceFactory::~ResourceFactory()
{

}

// IContent
HRESULT GDKAPI ResourceFactory::CreateTextureResource(_COM_Outptr_ ITextureResourceEdit** resource)
{
    if(!resource)
    {
        return E_INVALIDARG;
    }
    
    return Make<TextureResourceEdit>().CopyTo(resource);
}

HRESULT GDKAPI ResourceFactory::CreateGeometryResource(_COM_Outptr_ IGeometryResourceEdit** resource)
{
    if(!resource)
    {
        return E_INVALIDARG;
    }
    
    return Make<GeometryResourceEdit>().CopyTo(resource);
}

HRESULT GDKAPI ResourceFactory::CreateWorldResource(_COM_Outptr_ IWorldResourceEdit** resource)
{
    if(!resource)
    {
        return E_INVALIDARG;
    }
    
    return Make<WorldResourceEdit>().CopyTo(resource);
}

} // Content
} // GDK
