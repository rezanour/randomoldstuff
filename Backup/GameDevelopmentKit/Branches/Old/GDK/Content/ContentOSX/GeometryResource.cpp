#include "Precomp.h"

using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Content {

GeometryResource::GeometryResource(_In_ std::istream* stream)
{
    size_t startSeekPos = stream->tellg();
    stream->seekg(std::ios::end);
    size_t endSeekPos = stream->tellg();
    stream->seekg(startSeekPos);

    size_t dataLength = endSeekPos - startSeekPos;
    _data.reserve(dataLength);
    stream->read((char *)&_data.front(), dataLength);

    _header = (GEOMETRY_RESOURCE_FILEHEADER *)_data.data();
    byte_t* current = ((byte *)_header) + sizeof(GEOMETRY_RESOURCE_FILEHEADER);

    // Read frames
    for (size_t i = 0; i < _header->FrameCount; i++)
    {
        FrameData theFrame;
        
        theFrame._frame = (GEOMETRY_RESOURCE_FRAME*)current;
        current += sizeof(GEOMETRY_RESOURCE_FRAME);

        theFrame._vertices = current;
        current += (sizeof(GEOMETRY_RESOURCE_VERTEX) * theFrame._frame->VertexCount);

        theFrame._indices = (uint32_t*)current;
        current += (sizeof(uint32_t) * theFrame._frame->IndicesCount);

        // convert name from ansi to widechar
        theFrame._name = AnsiToWide(theFrame._frame->name);

        _frames.push_back(theFrame);
    }
}

// IGeometryResource
HRESULT GDKAPI GeometryResource::GetName(_Out_ const wchar_t** name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    *name = _resourceName.c_str();

    return S_OK;
}

HRESULT GDKAPI GeometryResource::GetFormat(_Out_ uint32_t* format)
{
    if (!format)
    {
        return E_INVALIDARG;
    }

    *format = _header->Format;

    return S_OK;
}

HRESULT GDKAPI GeometryResource::GetVertices(_Out_ uint32_t* count, _Out_ GEOMETRY_RESOURCE_VERTEX** vertices)
{
    return GetFrameVertices(0, count, vertices);
}

HRESULT GDKAPI GeometryResource::GetFrameName(_In_ uint32_t frame, _Out_ const wchar_t** name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    *name = _frames[frame]._name.c_str();

    return S_OK;
}

HRESULT GDKAPI GeometryResource::GetFrameVertices(_In_ uint32_t frame, _Out_ uint32_t* count, _Out_ GEOMETRY_RESOURCE_VERTEX** vertices)
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


HRESULT GDKAPI GeometryResource::GetIndices(_Out_ uint32_t* count, _Out_ uint32_t** indices)
{
    return GetFrameIndices(0, count, indices);
}

HRESULT GDKAPI GeometryResource::GetFrameIndices(_In_ uint32_t frame, _Out_ uint32_t* count, _Out_ uint32_t** indices)
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

HRESULT GDKAPI GeometryResource::GetFrameCount(_Out_ uint32_t* count)
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = (uint32_t)_frames.size();

    return S_OK;
}

std::wstring GeometryResource::AnsiToWide(std::string s)
{
    std::wstring ws(s.size(), L' ');
    std::copy(s.begin(), s.end(), ws.begin());
    return ws;
}

} // Content
} // GDK
