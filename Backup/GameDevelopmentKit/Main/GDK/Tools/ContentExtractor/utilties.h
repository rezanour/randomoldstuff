#pragma once

HRESULT GenerateNormals(GDK::GeometryContent::Vertex* vertices, size_t numVertices, uint32_t* indices, size_t numIndices);
HRESULT SeekTo(_In_ IStream* stream, _In_ DWORD dwOrigin, _In_ LONG offset);
DWORD GetStreamSize(_In_ IStream* stream);