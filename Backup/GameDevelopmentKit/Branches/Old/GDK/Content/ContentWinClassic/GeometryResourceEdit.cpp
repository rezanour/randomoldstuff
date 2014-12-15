#include "Precomp.h"

using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Content {

// IPersistResource
HRESULT GDKAPI GeometryResourceEdit::Save(_In_ GDK::IStream* output)
{
    UNREFERENCED_PARAMETER(output);
    return E_NOTIMPL;
}

HRESULT GDKAPI GeometryResourceEdit::Load(_In_ GDK::IStream* input)
{
    if(!input)
    {
        return E_INVALIDARG;
    }

    unsigned long bytesRead = 0;
    HRESULT hr = S_OK;
    STATSTG statstg = {0};
    byte_t* current = nullptr;
    hr = input->Stat(&statstg, STGTY_STREAM);
    _data.resize(statstg.cbSize.LowPart);

    hr = input->Read(_data.data(), (ULONG)_data.size(), &bytesRead);
    if (SUCCEEDED(hr))
    {
        // read the header
        _header = (GEOMETRY_RESOURCE_FILEHEADER*)_data.data();
        current = (byte_t*)_header;
        current += sizeof(GEOMETRY_RESOURCE_FILEHEADER);

        // Read frames
        for (size_t i = 0; i < _header->FrameCount; i++)
        {
            frameData theFrame;
            // Read Frame header
            if (SUCCEEDED(hr))
            {
                theFrame._frame = (GEOMETRY_RESOURCE_FRAME*)current;
                current += sizeof(GEOMETRY_RESOURCE_FRAME);

                theFrame._vertices = current;
                current += (sizeof(GEOMETRY_RESOURCE_VERTEX) * theFrame._frame->VertexCount);

                theFrame._indices = (uint32_t*)current;
                current += (sizeof(uint32_t) * theFrame._frame->IndicesCount);

                // convert name from ansi to widechar
                theFrame._name = AnsiToWide(theFrame._frame->name);
            }

            _frames.push_back(theFrame);
        }
    }

    return hr;
}

// IGeometryResourceEdit
HRESULT GDKAPI GeometryResourceEdit::SetName(_In_ const wchar_t* name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    _resourceName = name;

    return S_OK;
}

HRESULT GDKAPI GeometryResourceEdit::CreateGeometryResource(_COM_Outptr_ IGeometryResource** resource)
{
    UNREFERENCED_PARAMETER(resource);
    return E_NOTIMPL;
}

// IGeometryResource
HRESULT GDKAPI GeometryResourceEdit::GetName(_Out_ const wchar_t** name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    *name = _resourceName.c_str();

    return S_OK;
}

HRESULT GDKAPI GeometryResourceEdit::GetFormat(_Out_ uint32_t* format)
{
    if (!format)
    {
        return E_INVALIDARG;
    }

    *format = _header->Format;

    return S_OK;
}

HRESULT GDKAPI GeometryResourceEdit::GetVertices(_Out_ uint32_t* count, _Out_ const GEOMETRY_RESOURCE_VERTEX** vertices)
{
    return GetFrameVertices(0, count, vertices);
}

HRESULT GDKAPI GeometryResourceEdit::GetFrameName(_In_ uint32_t frame, _Out_ const wchar_t** name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    *name = _frames[frame]._name.c_str();

    return S_OK;
}

HRESULT GDKAPI GeometryResourceEdit::GetFrameVertices(_In_ uint32_t frame, _Out_ uint32_t* count, _Out_ const GEOMETRY_RESOURCE_VERTEX** vertices)
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    if (vertices)
    {
        *vertices = (GEOMETRY_RESOURCE_VERTEX*)_frames[frame]._vertices;
    }

    if (count)
    {
        *count = _frames[frame]._frame->VertexCount;
    }

    return S_OK;
}


HRESULT GDKAPI GeometryResourceEdit::GetIndices(_Out_ uint32_t* count, _Out_ const uint32_t** indices)
{
    return GetFrameIndices(0, count, indices);
}

HRESULT GDKAPI GeometryResourceEdit::GetFrameIndices(_In_ uint32_t frame, _Out_ uint32_t* count, _Out_ const uint32_t** indices)
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    if (indices)
    {
        *indices = _frames[frame]._indices;
    }

    if (count)
    {
        *count = _frames[frame]._frame->IndicesCount;
    }

    return S_OK;
}

HRESULT GDKAPI GeometryResourceEdit::GetFrameCount(_Out_ uint32_t* count)
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = (uint32_t)_frames.size();

    return S_OK;
}

GeometryResourceEdit::GeometryResourceEdit()
{

}

GeometryResourceEdit::~GeometryResourceEdit()
{

}

std::wstring GeometryResourceEdit::AnsiToWide(std::string s)
{
    std::wstring ws(s.size(), L' ');
    std::copy(s.begin(), s.end(), ws.begin());
    return ws;
}

} // Content
} // GDK
