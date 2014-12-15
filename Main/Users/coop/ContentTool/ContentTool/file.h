#pragma once

void ReadFileToBuffer(_In_ std::wstring path, _Inout_ std::vector<uint8_t>& buffer);
void WriteBufferToFile(_In_ std::wstring path, _In_ std::vector<uint8_t>& buffer);
void WriteBufferToFile(_In_ std::wstring path, _In_reads_bytes_(bufferSize) uint8_t* buffer, _In_ uint32_t bufferSize);
void WriteTextureToFile(_In_ std::wstring path, _In_ std::shared_ptr<GDK::TextureContent>& content);
void WriteGeometryToFile(_In_ std::wstring path, _In_ std::shared_ptr<GDK::GeometryContent>& content);
void WriteGeometryToObjFile(_In_ std::wstring path, _In_ std::shared_ptr<GDK::GeometryContent>& content);