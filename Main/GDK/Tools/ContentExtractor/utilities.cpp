#include "stdafx.h"

HRESULT GenerateNormals(const byte_t* positions, size_t stride, size_t numVertices, const uint32_t* indices, size_t numIndices, _Inout_ byte_t* normals)
{
    if (!positions || !indices || (numIndices == 0) || (numVertices == 0))
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    // Copied code from Lucid Graphics system
    const void*  pFirstPosition = positions;
    void*  pFirstNormal   = normals;
    const void*  pFirstIndex    = indices;
    size_t indexStride    = sizeof(uint32_t);
    bool   flipCross      = false;

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

    return hr;
}

HRESULT SeekTo(_In_ IStream* stream, _In_ DWORD dwOrigin, _In_ LONG offset)
{            
    LARGE_INTEGER liMove = {0};
    ULARGE_INTEGER uliPos = {0};
    liMove.LowPart = offset;

    return stream->Seek(liMove, dwOrigin, &uliPos);
}

DWORD GetStreamSize(_In_ IStream* stream)
{
    STATSTG statstg;
    DWORD streamSize = 0;
    stream->Stat(&statstg, STATFLAG_DEFAULT);

    return statstg.cbSize.LowPart;
}

std::wstring AnsiToWide(std::string s)
{
    std::wstring ws(s.size(), L' ');
    std::copy(s.begin(), s.end(), ws.begin());
    return ws;
}

std::wstring DirectoryRootFromPath(std::wstring path)
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
