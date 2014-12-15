#include "Precomp.h"
#include <fstream>

using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Content {

// IContent
HRESULT GDKAPI ContentArchive::GetGeometryResource(_In_z_ const wchar_t* name, _COM_Outptr_ IGeometryResource** resource)
{
    std::wstring nameString = name;
    std::string asciiNameString;
    asciiNameString.assign(nameString.begin(), nameString.end());

    std::ifstream stream;
    stream.open(asciiNameString);
    
    try
    {
        *resource = GDK::Make<GeometryResource>(&stream).Detach();
        stream.close();
        return S_OK;
    }
    catch (GDK::Exception ex)
    {
        *resource = nullptr;
        stream.close();
        return ex.GetResult();
    }
}

HRESULT GDKAPI ContentArchive::GetTextureResource(_In_z_ const wchar_t* name, _COM_Outptr_ ITextureResource** resource)
    {
        std::wstring nameString = name;
        std::string asciiNameString;
        asciiNameString.assign(nameString.begin(), nameString.end());

        std::ifstream stream;
        stream.open(asciiNameString);

        try
        {
            *resource = GDK::Make<TextureResource>(&stream).Detach();
            stream.close();
            return S_OK;
        }
        catch (GDK::Exception ex)
        {
            *resource = nullptr;
            stream.close();
            return ex.GetResult();
        }
}
    
} // Content
} // GDK
