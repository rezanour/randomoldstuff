#pragma once

namespace GDK {
namespace Content {

    class GeometryResourceEdit : public RuntimeObject<IGeometryResourceEdit, IGeometryResource>
    {
    public:
        static Microsoft::WRL::ComPtr<GeometryResourceEdit> Create();
        GeometryResourceEdit();

        // IPersistResource
        virtual HRESULT GDKAPI Save(_In_ GDK::IStream* output);
        virtual HRESULT GDKAPI Load(_In_ GDK::IStream* input);

        // IGeometryResourceEdit
        virtual HRESULT GDKAPI SetName(_In_ const wchar_t* name);
        virtual HRESULT GDKAPI CreateGeometryResource(_COM_Outptr_ IGeometryResource** resource);

        // IGeometryResource
        virtual HRESULT GDKAPI GetName(_Out_ const wchar_t** name);
        virtual HRESULT GDKAPI GetFormat(_Out_ uint32_t* format);
        virtual HRESULT GDKAPI GetVertices(_Out_ uint32_t* count, _Out_ const GEOMETRY_RESOURCE_VERTEX** vertices);
        virtual HRESULT GDKAPI GetIndices(_Out_ uint32_t* count, _Out_ const uint32_t** indices);

        virtual HRESULT GDKAPI GetFrameCount(_Out_ uint32_t* count);
        virtual HRESULT GDKAPI GetFrameName(_In_ uint32_t frame, _Out_ const wchar_t** name);
        virtual HRESULT GDKAPI GetFrameVertices(_In_ uint32_t frame, _Out_ uint32_t* count, _Out_ const GEOMETRY_RESOURCE_VERTEX** vertices);
        virtual HRESULT GDKAPI GetFrameIndices(_In_ uint32_t frame, _Out_ uint32_t* count, _Out_ const uint32_t** indices);
        
    private:
        ~GeometryResourceEdit();
        std::wstring AnsiToWide(std::string s);

        struct frameData
        {
            GEOMETRY_RESOURCE_FRAME* _frame;
            byte_t*   _vertices;
            uint32_t* _indices;
            std::wstring _name;
        };

        GEOMETRY_RESOURCE_FILEHEADER* _header;
        std::vector<frameData> _frames;
        std::vector<byte_t> _data;
        std::wstring _resourceName;
    };

} // Content
} // GDK
