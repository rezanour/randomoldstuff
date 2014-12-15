#pragma once

namespace GDK {
namespace Content {

    class ResourceFactory : public RuntimeObject<IResourceFactory>
    {
    public:
        ResourceFactory();

        // IResourceFactory
        virtual HRESULT GDKAPI CreateTextureResource(_COM_Outptr_ ITextureResourceEdit** resource);
        virtual HRESULT GDKAPI CreateGeometryResource(_COM_Outptr_ IGeometryResourceEdit** resource);
        virtual HRESULT GDKAPI CreateWorldResource(_COM_Outptr_ IWorldResourceEdit** resource);
        
    private:
        ~ResourceFactory();
    };

} // Content
} // GDK
