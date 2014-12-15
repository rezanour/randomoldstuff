#include <stdint.h>
#include "helpers.h"

#include <DirectXMath.h> // Included for GenerateNormals helper which needs to be rewritten using GDK math types

#ifdef WIN32
    // Only include windows.h for ::CreateDirectory and ::GetLastError when compiling for
    // Windows platforms.
    #include <Windows.h>
#endif

std::wstring AnsiToWide(_In_ const std::string& s)
{
    std::wstring ws(s.size(), L' ');
    std::copy(s.begin(), s.end(), ws.begin());
    return ws;
}

void NormalizePathSlashes(_Inout_ std::string& s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == '/')
        {
            s[i] = '\\';
        }
    }
}

void NormalizePathSlashes(_Inout_ std::wstring& s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == L'/')
        {
            s[i] = L'\\';
        }
    }
}

void ReplaceCharacter(_Inout_ std::wstring& s, _In_ wchar_t fromCh, _In_ wchar_t toCh)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == fromCh)
        {
            s[i] = toCh;
        }
    }
}

std::wstring DirectoryRootFromPath(_In_ const std::wstring& path)
{
    std::wstring root;
    size_t pos = path.find_last_of(L"\\");
    if (pos != std::wstring::npos)
    {
        root = path.substr(0, pos);
    }
    else
    {
        root = path;
    }

    return root;
}

std::wstring FileNameFromPath(_In_ const std::wstring& path)
{
    std::wstring fileName;
    size_t pos = path.rfind(L"\\");
    if (pos != std::wstring::npos)
    {
        fileName = path.substr(pos + 1);
    }
    else
    {
        fileName = path;
    }

    return fileName;
}

void CreateFullDirectory(_In_ const std::wstring& path)
{
#ifndef WIN32
    #error CreateFullDirectory is not implemented.
#else
    uint32_t errorCode = ERROR_SUCCESS;
    size_t slash = path.find_first_of(L'\\');
    while (slash != std::wstring::npos)
    {
        std::wstring subPath = path.substr(0, slash);
        if (!::CreateDirectory(subPath.c_str(), nullptr))
        {
            errorCode = ::GetLastError();
            if (errorCode != ERROR_ALREADY_EXISTS)
            {
                return;
            }
        }

        slash = path.find_first_of(L'\\', slash + 1);
    }

    if (!::CreateDirectory(path.c_str(), nullptr))
    {
        errorCode = ::GetLastError();
        if (errorCode != ERROR_ALREADY_EXISTS)
        {
            return;
        }
    }
#endif
}

void GenerateNormals(_In_ const uint8_t* positions, _In_ size_t stride, _In_ size_t numVertices, _In_ const uint32_t* indices, _In_ size_t numIndices, _Inout_ uint8_t* normals)
{
    const void* pFirstPosition = positions;
    void* pFirstNormal = normals;
    const void* pFirstIndex = indices;
    size_t indexStride = sizeof(uint32_t);
    bool flipCross = false;

    {
        bool fullSizeIndices = (indexStride >= 4);

        byte* pIndices = (byte*)pFirstIndex;
        byte* pPositions = (byte*)pFirstPosition;
        byte* pNormals = (byte*)pFirstNormal;

        uint32_t i0, i1, i2;
        DirectX::XMVECTOR v0, v1, v2, normal;

        // zero pass
        for (size_t i = 0; i < numVertices; i++)
        {
            *(DirectX::XMFLOAT3*)(pNormals + (i * stride)) = DirectX::XMFLOAT3(0, 0, 0);
        }

        for (size_t i = 0; i < numIndices; i+=3)
        {
            if (fullSizeIndices)
            {
                i0 = *(uint32_t*)(pIndices + (i * indexStride));
                i1 = *(uint32_t*)(pIndices + ((i + 1) * indexStride));
                i2 = *(uint32_t*)(pIndices + ((i + 2) * indexStride));
            }
            else
            {
                i0 = (uint32_t)(*(uint16_t*)(pIndices + (i * indexStride)));
                i1 = (uint32_t)(*(uint16_t*)(pIndices + ((i + 1) * indexStride)));
                i2 = (uint32_t)(*(uint16_t*)(pIndices + ((i + 2) * indexStride)));
            }

            v0 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pPositions + (i0 * stride)));
            v1 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pPositions + (i1 * stride)));
            v2 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pPositions + (i2 * stride)));

            DirectX::XMFLOAT3* n0 = (DirectX::XMFLOAT3*)(pNormals + (i0 * stride));
            DirectX::XMFLOAT3* n1 = (DirectX::XMFLOAT3*)(pNormals + (i1 * stride));
            DirectX::XMFLOAT3* n2 = (DirectX::XMFLOAT3*)(pNormals + (i2 * stride));

            normal = flipCross ? 
                            DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v2, v0), DirectX::XMVectorSubtract(v1, v0))) :
                            DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v1, v0), DirectX::XMVectorSubtract(v2, v0)));

            DirectX::XMStoreFloat3(n0, DirectX::XMVectorAdd(XMLoadFloat3(n0), normal));
            DirectX::XMStoreFloat3(n1, DirectX::XMVectorAdd(XMLoadFloat3(n1), normal));
            DirectX::XMStoreFloat3(n2, DirectX::XMVectorAdd(XMLoadFloat3(n2), normal));
        }

        // normalize pass
        for (size_t i = 0; i < numVertices; i++)
        {
            DirectX::XMFLOAT3* n = (DirectX::XMFLOAT3*)(pNormals + (i * stride));
            DirectX::XMStoreFloat3(n, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(n)));
        }
    }
}