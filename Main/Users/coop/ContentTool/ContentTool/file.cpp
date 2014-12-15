#include "stdafx.h"

void ReadFileToBuffer(_In_ std::wstring path, _Inout_ std::vector<uint8_t>& buffer)
{
    std::ifstream inputfile(path,std::ios::in|std::ios_base::binary|std::ios::ate);
    if (inputfile.is_open())
    {
        buffer.resize((uint32_t)inputfile.tellg());
        inputfile.seekg(0, std::ios::beg);
        inputfile.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        inputfile.close();
    }
}

void WriteBufferToFile(_In_ std::wstring path, _In_ std::vector<uint8_t>& buffer)
{
    std::ofstream outputfile(path,std::ios_base::binary);
    if (outputfile.is_open())
    {
        outputfile.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
        outputfile.close();
    }
}

void WriteBufferToFile(_In_ std::wstring path, _In_reads_bytes_(bufferSize) uint8_t* buffer, _In_ uint32_t bufferSize)
{
    std::ofstream outputfile(path,std::ios_base::binary);
    if (outputfile.is_open())
    {
        outputfile.write(reinterpret_cast<char*>(buffer), bufferSize);
        outputfile.close();
    }
}

void WriteTextureToFile(_In_ std::wstring path, _In_ std::shared_ptr<GDK::TextureContent>& content)
{
    std::ofstream outputfile(path,std::ios_base::binary);
    if (outputfile.is_open())
    {
        content->Save(outputfile);
        outputfile.close();
    }
}

void WriteGeometryToFile(_In_ std::wstring path, _In_ std::shared_ptr<GDK::GeometryContent>& content)
{
    std::ofstream outputfile(path,std::ios_base::binary);
    if (outputfile.is_open())
    {
        content->Save(outputfile);
        outputfile.close();
    }
}

void WriteGeometryToObjFile(_In_ std::wstring path, _In_ std::shared_ptr<GDK::GeometryContent>& content)
{
    SaveAsWaveFrontOBJ(path.c_str(), content->GetVertices(), content->GetVertexStride(), content->GetNumVertices(), content->GetIndices(), content->GetNumIndices() );
}
