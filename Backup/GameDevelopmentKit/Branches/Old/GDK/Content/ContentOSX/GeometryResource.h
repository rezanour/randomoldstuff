#pragma once

namespace GDK {
namespace Content {

    class GeometryResource : public RuntimeObject<IGeometryResource>
    {
    public:
        explicit GeometryResource(_In_ std::istream* stream);

        // IGeometryResource
        virtual HRESULT GDKAPI GetName(_Out_ const wchar_t** name);
        virtual HRESULT GDKAPI GetFormat(_Out_ uint32_t* format);
        virtual HRESULT GDKAPI GetVertices(_Out_ uint32_t* count, _Out_ GEOMETRY_RESOURCE_VERTEX** vertices);
        virtual HRESULT GDKAPI GetIndices(_Out_ uint32_t* count, _Out_ uint32_t** indices);

        virtual HRESULT GDKAPI GetFrameCount(_Out_ uint32_t* count);
        virtual HRESULT GDKAPI GetFrameName(_In_ uint32_t frame, _Out_ const wchar_t** name);
        virtual HRESULT GDKAPI GetFrameVertices(_In_ uint32_t frame, _Out_ uint32_t* count, _Out_ GEOMETRY_RESOURCE_VERTEX** vertices);
        virtual HRESULT GDKAPI GetFrameIndices(_In_ uint32_t frame, _Out_ uint32_t* count, _Out_ uint32_t** indices);
        
    private:
        std::wstring AnsiToWide(std::string s);

        struct FrameData
        {
            GEOMETRY_RESOURCE_FRAME* _frame;
            byte_t* _vertices;
            uint32_t* _indices;
            std::wstring _name;
        };

        GEOMETRY_RESOURCE_FILEHEADER* _header;
        std::vector<FrameData> _frames;
        std::vector<byte_t> _data;
        std::wstring _resourceName;
    };

} // Content
} // GDK
