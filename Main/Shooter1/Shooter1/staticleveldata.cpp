#include "precomp.h"
#include "staticleveldata.h"
#include "bih.h"
#include "bspcompiler.h"
#include "texturestreamer.h"
#include "kdtree.h"
#include "kdtree2.h"

_Use_decl_annotations_
StaticLevelData::StaticLevelData(const ComPtr<ID3D11DeviceContext>& context, const StaticGeometryVertex* vertices, uint32_t numVertices,
                                 const uint32_t* indices, uint32_t numIndices, std::unique_ptr<MaterialSource[]>& materials, uint32_t numMaterials,
                                 size_t textureBudgetBytes) :
    _context(context), _numMaterials(numMaterials), _numVisibleIndices(0), _numIndices(numIndices), _numVisibleMaterials(0), _numResident(0)
{
    //_spatial.reset(BspCompiler::CreateFromTriangles(vertices, numVertices, indices, numIndices));
    _spatial.reset(BIH::CreateFromTriangles(vertices, numVertices, indices, numIndices));
    //_spatial.reset(KdTreeCompiler2::CreateFromTriangles(vertices, numVertices, indices, numIndices));
    //{
    //    std::unique_ptr<KdTreeCompiler> kdTree(KdTreeCompiler::CreateFromTriangles(vertices, numVertices, indices, numIndices));
    //}

    ComPtr<ID3D11Device> device;
    context->GetDevice(&device);

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.StructureByteStride = sizeof(StaticGeometryVertex);
    bd.ByteWidth = bd.StructureByteStride * numVertices;
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = vertices;
    init.SysMemPitch = bd.ByteWidth;

    CHECKHR(device->CreateBuffer(&bd, &init, &_vertices));

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.StructureByteStride = sizeof(uint32_t);
    bd.ByteWidth = bd.StructureByteStride * numIndices;

    init.pSysMem = indices;
    init.SysMemPitch = bd.ByteWidth;

    CHECKHR(device->CreateBuffer(&bd, &init, &_indices));

    _visibleIndices.reset(new uint32_t[numIndices]);
    _visibleMaterials.reset(new uint32_t[numIndices]);

    //
    // Determine size of texture array and texture resolution based on number of textures we
    // need to handle, and the texture budget we've been given. Equation formulated offline, and
    // is based on caching mip level 2-n for all textures, and then keeping a smaller number
    // of resident full textures, including mips 0 & 1.
    //
    // 3/4 * (budget / mip0Size) - 3/48 * totalTextures = textureArraySize
    //
    // We start by trying with the largest mip size we support (1024x1024), and compute textureArraySize.
    // If this value is enough, then we stop. Otherwise, we move down to the next mip resolution as our target
    // and recompute. We continue iterating until we find one that allows us to hold enough concurrent textures
    // resident at the current budget. That mip level is our top texture resolution for the level.
    //

    // Assuming DXGI_FORMAT_R8G8B8A8_UNORM for now. If we decide to move to another format, we need to adjust
    uint32_t numTextures = numMaterials * 2 + 1; // diffuse & normals, plus 1 for error texture
    uint32_t bytesPerPixel = 4;
    uint32_t size = 1024;
    uint32_t mip0Size = size * size * bytesPerPixel;
    static const float c1 = 3.0f / 48.0f;
    _textureArraySize = (uint32_t)(0.75f * (textureBudgetBytes / (size_t)mip0Size) - c1 * numTextures);

    uint32_t minTextureArraySize = 32;
    uint32_t initialMip = 0;

    while (_textureArraySize < minTextureArraySize && size > 16)
    {
        size /= 2;
        mip0Size = size * size * bytesPerPixel;
        _textureArraySize = (uint32_t)(0.75f * (textureBudgetBytes / (size_t)mip0Size) - c1 * numTextures);
        ++initialMip;
    }

    if (size == 16)
    {
        Error("Could not satisfy the number of textures required with the budget provided.");
    }

    D3D11_TEXTURE2D_DESC td = {};
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R32G32_UINT;
    td.Width = numMaterials;
    td.Height = 1;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DYNAMIC;
    td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ComPtr<ID3D11Texture2D> texture;
    CHECKHR(device->CreateTexture2D(&td, nullptr, &texture));
    CHECKHR(device->CreateShaderResourceView(texture.Get(), nullptr, &_materialMap));

    td.ArraySize = _textureArraySize;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.Height = size;
    td.Width = size;
    td.MipLevels = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.CPUAccessFlags = 0;

    CHECKHR(device->CreateTexture2D(&td, nullptr, &texture));
    CHECKHR(device->CreateShaderResourceView(texture.Get(), nullptr, &_textureArray));

    _mapEntries.reset(new MaterialMapEntry[numMaterials]);
    ZeroMemory(_mapEntries.get(), numMaterials * sizeof(MaterialMapEntry));

    _residency.reset(new MaterialResidency[numMaterials]);
    ZeroMemory(_residency.get(), numMaterials * sizeof(MaterialResidency));

    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.StructureByteStride = sizeof(PerFrame);
    bd.ByteWidth = bd.StructureByteStride;

    CHECKHR(device->CreateBuffer(&bd, nullptr, &_perFrameCB));

    D3D11_INPUT_ELEMENT_DESC elems[4] = {};

    elems[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[0].SemanticName = "POSITION";
    elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    elems[1].AlignedByteOffset = sizeof(XMFLOAT3);
    elems[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[1].SemanticName = "NORMAL";
    elems[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    elems[2].AlignedByteOffset = 2 * sizeof(XMFLOAT3);
    elems[2].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[2].SemanticName = "TEXCOORD";
    elems[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    elems[3].AlignedByteOffset = 2 * sizeof(XMFLOAT3) + sizeof(XMFLOAT2);
    elems[3].Format = DXGI_FORMAT_R32_UINT;
    elems[3].SemanticName = "TEXCOORD";
    elems[3].SemanticIndex = 1;
    elems[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    size_t length;
    auto buffer = ReadFile("staticlevel_vs.cso", &length);

    CHECKHR(device->CreateVertexShader(buffer.get(), length, nullptr, &_vertexShader));
    CHECKHR(device->CreateInputLayout(elems, _countof(elems), buffer.get(), length, &_inputLayout));

    buffer = ReadFile("staticlevel_ps.cso", &length);

    CHECKHR(device->CreatePixelShader(buffer.get(), length, nullptr, &_pixelShader));

    _textureStreamer.reset(new TextureStreamer(materials, numMaterials, texture, initialMip));
}

StaticLevelData::~StaticLevelData()
{
    // Shutdown streamer first (blocks until flushed)
    _textureStreamer = nullptr;
}

void StaticLevelData::Draw(const XMFLOAT4X4& cameraWorld, const XMFLOAT4X4& view, const XMFLOAT4X4& projection)
{
    {
        auto lock = GetGraphics().LockContext();

        //
        // Prepare for rendering the level
        //

        static const uint32_t stride = sizeof(StaticGeometryVertex);
        static const uint32_t offset = 0;

        _context->IASetVertexBuffers(0, 1, _vertices.GetAddressOf(), &stride, &offset);
        _context->IASetIndexBuffer(_indices.Get(), DXGI_FORMAT_R32_UINT, 0);
        _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _context->IASetInputLayout(_inputLayout.Get());
        _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
        _context->PSSetShader(_pixelShader.Get(), nullptr, 0);

        _context->VSSetConstantBuffers(0, 1, _perFrameCB.GetAddressOf());
        _context->VSSetShaderResources(0, 1, _materialMap.GetAddressOf());

        _context->PSSetShaderResources(0, 1, _textureArray.GetAddressOf());
        _context->PSSetSamplers(0, 1, GetGraphics().GetLinearWrapSampler().GetAddressOf());
    }

    //
    // Gather the visible indices
    //

    //UNREFERENCED_PARAMETER(cameraWorld);
    _spatial->BuildVisibleIndexList(XMLoadFloat4x4(&cameraWorld), XMLoadFloat4x4(&projection), _visibleIndices.get(), _numIndices, &_numVisibleIndices, _visibleMaterials.get(), _numMaterials, &_numVisibleMaterials);

    //
    // Ensure materials are paged in and update the IB
    //

    {
        auto lock = GetGraphics().LockContext();

        uint32_t* materialId = _visibleMaterials.get();
        for (uint32_t i = 0; i < _numVisibleMaterials; ++i, ++materialId)
        {
            EnsureMaterial(*materialId);
        }

        ComPtr<ID3D11Resource> resource;
        _materialMap->GetResource(&resource);

        D3D11_MAPPED_SUBRESOURCE mapped;
        CHECKHR(_context->Map(resource.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
        memcpy_s(mapped.pData, mapped.RowPitch, _mapEntries.get(), sizeof(MaterialMapEntry) * _numMaterials);
        _context->Unmap(resource.Get(), 0);

        _perFrame.View = view;
        _perFrame.Projection = projection;
        _context->UpdateSubresource(_perFrameCB.Get(), 0, nullptr, &_perFrame, sizeof(_perFrame), 0);

        D3D11_BOX box = {};
        box.right = _numVisibleIndices * sizeof(uint32_t);
        box.bottom = 1;
        box.back = 1;

        _context->UpdateSubresource(_indices.Get(), 0, &box, _visibleIndices.get(), box.right, box.right);

        _context->DrawIndexed(_numVisibleIndices, 0, 0);
    }
}

void StaticLevelData::EnsureMaterial(uint32_t materialId)
{
    assert(materialId < _numMaterials);

    MaterialMapEntry& mapEntry = _mapEntries.get()[materialId];
    MaterialResidency& residency = _residency.get()[materialId];

    if (!residency.Resident)
    {
        uint32_t index = (uint32_t)-1;

        // If we haven't reached full resident count, then just grab the next empty slot
        if (_numResident < _textureArraySize)
        {
            index = _numResident++;
        }
        else
        {
            // Otherwise, find the oldest resident material and replace it

            // TODO: optimize this!
            uint64_t oldestFrameNumber = UINT64_MAX;
            MaterialResidency* res = _residency.get();
            uint32_t i = 0;
            for (; i < _numMaterials; ++i, ++res)
            {
                if (i == materialId)
                {
                    continue;
                }

                if (res->Resident && res->LastFrameUsed < oldestFrameNumber)
                {
                    oldestFrameNumber = res->LastFrameUsed;
                    index = _mapEntries.get()[i].Index;
                }
            }

            MaterialResidency& otherRes = _residency.get()[i];
            otherRes.Resident = false;
        }

        assert(index != (uint32_t)-1);

        mapEntry.Index = index;
        _textureStreamer->RequestMaterial(materialId, index, &mapEntry.MipBias);

        residency.Resident = true;
    }

    static uint64_t FrameNumber = 0;
    residency.LastFrameUsed = FrameNumber++;
}
