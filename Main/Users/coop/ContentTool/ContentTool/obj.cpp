#include "stdafx.h"

using namespace Microsoft::WRL;

HRESULT SaveAsWaveFrontOBJ(const wchar_t* path, const uint8_t* positions, size_t stride, size_t numVertices, const uint32_t* indices, size_t numIndices )
{
    if (!path || !positions || !numVertices || !indices || !numIndices)
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
    sprintf_s(temp, objHeader);
    objFileData.append(temp);

    // Write object name
    ZeroMemory(temp, sizeof(temp));
    sprintf_s(temp, objName, "GeometryResource");
    objFileData.append(temp);

    uint8_t* p = (uint8_t*)positions;
    Vector3* pos = (Vector3*)(p);

    for (size_t v = 0; v < numVertices; v++)
    {
        // Write vertex
        ZeroMemory(temp, sizeof(temp));
        pos = (Vector3*)(p);
        Vector3 position = *pos;
        sprintf_s(temp, objVertex, position.x, position.y, position.z);
        objFileData.append(temp);
        p += stride;
    }

    for (size_t i = 0; i < numIndices; i+=3)
    {
        // Write Indices
        ZeroMemory(temp, sizeof(temp));
        sprintf_s(temp, objTriangle, indices[i]+1, indices[i+1]+1, indices[i+2]+1);
        objFileData.append(temp);
    }

    WriteBufferToFile(path, (uint8_t*)objFileData.data(), (uint32_t)objFileData.size() * sizeof(char));

    return S_OK;
}
