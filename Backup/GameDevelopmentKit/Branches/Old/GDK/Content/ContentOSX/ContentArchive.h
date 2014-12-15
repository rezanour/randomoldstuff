#pragma once

namespace GDK {
namespace Content {

    class ContentArchive : public RuntimeObject<IContent>
    {
    public:
        // IContent
        virtual HRESULT GDKAPI GetGeometryResource(_In_z_ const wchar_t* name, _COM_Outptr_ IGeometryResource** resource);
        virtual HRESULT GDKAPI GetTextureResource(_In_z_ const wchar_t* name, _COM_Outptr_ ITextureResource** resource);
        
    private:
    };

} // Content
} // GDK
