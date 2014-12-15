#pragma once
#include <string>

std::wstring AnsiToWide(_In_ const std::string& s);
void NormalizePathSlashes(_Inout_ std::string& s);
void NormalizePathSlashes(_Inout_ std::wstring& s);
std::wstring DirectoryRootFromPath(_In_ const std::wstring& path);
std::wstring FileNameFromPath(_In_ const std::wstring& path);
void ReplaceCharacter(_Inout_ std::wstring& s, _In_ wchar_t fromCh, _In_ wchar_t toCh);
void CreateFullDirectory(_In_ const std::wstring& path);
void GenerateNormals(_In_ const uint8_t* positions, _In_ size_t stride, _In_ size_t numVertices, _In_ const uint32_t* indices, _In_ size_t numIndices, _Inout_ uint8_t* normals);
