#include "stdafx.h"

void __cdecl plugindprintf(const char *format, ...)
{
    char    buf[4096], *p = buf;
    va_list args;
    int     n;

    va_start(args, format);
    n = _vsnprintf(p, sizeof buf - 3, format, args); // buf-3 is room for CR/LF/NUL
    va_end(args);

    p += (n < 0) ? sizeof buf - 3 : n;

    while ( p > buf  &&  isspace(p[-1]) )
            *--p = '\0';

    *p++ = '\r';
    *p++ = '\n';
    *p   = '\0';

    OutputDebugStringA(buf);
}

void TriangulateFace(std::vector<XMPosTexNormVertexId>& face, bool swapWindingOrder)
{
    size_t numVertices = face.size();
    DWORD missingVertices = numVertices % 3;
    if (missingVertices != 0)
    {
        std::vector<XMPosTexNormVertexId> tface;
        for (UINT i = 0; i < face.size() - 2; i++)
        {
            if (swapWindingOrder)
            {
                tface.push_back(face[i + 2]);
                tface.push_back(face[i + 1]);
                tface.push_back(face[0]);
            }
            else
            {
                tface.push_back(face[0]);
                tface.push_back(face[i + 1]);
                tface.push_back(face[i + 2]);
            }
        }

        face.clear();
        for (UINT i = 0; i < tface.size(); i++)
        {
            face.push_back(tface[i]);
        }
    }

    //DEBUG_PRINT("Face had %d vertices, now has %d vertices", totalPolyCount, face.indices.size());
}

void ScaleVertices(XMPosNormTexVertex* vertices, UINT numVertices, float scale)
{
    for (UINT i = 0; i < numVertices; i++)
    {
        vertices[i].position.x *= scale;
        vertices[i].position.y *= scale;
        vertices[i].position.z *= scale;
    }
}

void NormalizeVector3(XMFLOAT3& vector3)
{
    float length = 0.0f;

    length = (float)sqrt((vector3.x*vector3.x) + (vector3.y*vector3.y) + (vector3.z*vector3.z));

    if(length == 0.0f)
        length = 1.0f;

    vector3.x /= length;
    vector3.y /= length;
    vector3.z /= length;
}

void NormalizeVector2(XMFLOAT2& vector2)
{
    float length = 0.0f;

    length = (float)sqrt((vector2.x*vector2.x) + (vector2.y*vector2.y));

    if(length == 0.0f)
        length = 1.0f;

    vector2.x /= length;
    vector2.y /= length;
}

XMFLOAT3 CalculateNormal(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3)
{
    XMFLOAT3 normal;
    XMFLOAT3 v1;
    XMFLOAT3 v2;

    // Calculate The Vector From Point 2 To Point 1 
    v1.x = p1.x - p2.x;
    v1.y = p1.y - p2.y;
    v1.z = p1.z - p2.z;
    
    // Calculate The Vector From Point 3 To Point 2 
    v2.x = p2.x - p3.x;
    v2.y = p2.y - p3.y;
    v2.z = p2.z - p3.z;

    // Compute the Cross Product to get a face Normal 
    normal.x = v1.y * v2.z - v1.z * v2.y;
    normal.y = v1.z * v2.x - v1.x * v2.z;
    normal.z = v1.x * v2.y - v1.y * v2.x;
 
    // Normalize the result
    NormalizeVector3(normal);

    return normal;
}
