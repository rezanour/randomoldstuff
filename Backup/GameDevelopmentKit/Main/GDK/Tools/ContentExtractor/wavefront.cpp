#include "stdafx.h"
using namespace Microsoft::WRL;

HRESULT SaveAsWaveFrontOBJ(const wchar_t* path, GDK::GeometryContent::Vertex* vertices, size_t numVertices, uint32_t* indices, size_t numIndices )
{
    if (!path || !vertices || !numVertices || !indices || !numIndices)
    {
        return E_INVALIDARG;
    }

    const char* objHeader =   "# GDK Geometry Resource Conversion 1.00\r\n";
    const char* objName =     "o %s\r\n";
    const char* objVertex =   "v %f %f %f\r\n";
    const char* objTriangle = "f %d %d %d\r\n";

    HRESULT hr = S_OK;
    ComPtr<IStream> objFile;
    std::string objFileData;
    ULONG bytesWritten = 0;
    char temp[255];

    // Write header
    ZeroMemory(temp, sizeof(temp));
    sprintf(temp, objHeader);
    objFileData.append(temp);

    // Write object name
    ZeroMemory(temp, sizeof(temp));
    sprintf(temp, objName, "GeometryResource");
    objFileData.append(temp);

    for (size_t v = 0; v < numVertices; v++)
    {
        // Write vertex
        ZeroMemory(temp, sizeof(temp));
        sprintf(temp, objVertex, vertices[v].Position.x, vertices[v].Position.y, vertices[v].Position.z);
        objFileData.append(temp);
    }

    for (size_t i = 0; i < numIndices; i+=3)
    {
        // Write Indices
        ZeroMemory(temp, sizeof(temp));
        sprintf(temp, objTriangle, indices[i]+1, indices[i+1]+1, indices[i+2]+1);
        objFileData.append(temp);
    }

    CHECKHR(SHCreateStreamOnFile(path, STGM_CREATE|STGM_READWRITE, &objFile), L"Error creating wavefront obj file");
    CHECKHR(objFile->Write(objFileData.data(), (ULONG)objFileData.size() * sizeof(char), &bytesWritten), L"Error writing wavefront obj data");

Exit:

    return hr;
}
