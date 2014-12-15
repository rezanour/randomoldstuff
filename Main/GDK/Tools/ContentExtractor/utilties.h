#pragma once

HRESULT GenerateNormals(const byte_t* positions, size_t stride, size_t numVertices, const uint32_t* indices, size_t numIndices, _Inout_ byte_t* normals);
HRESULT SeekTo(_In_ IStream* stream, _In_ DWORD dwOrigin, _In_ LONG offset);
DWORD GetStreamSize(_In_ IStream* stream);
std::wstring AnsiToWide(std::string s);
std::wstring DirectoryRootFromPath(std::wstring path);