#include "precomp.h"
#include "draw2d.h"
#include "material.h"

_Use_decl_annotations_
Draw2D::Draw2D(ID3D11DeviceContext* context)
    : _context(context), _numInstances(0)
{
    _maxInstancesPerBatch = 1024; // TODO: determine this based on specs/settings

    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    D3D11_INPUT_ELEMENT_DESC elems[5] = {};

    elems[0].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[0].SemanticName = "POSITION";

    elems[1].InputSlot = 1;
    elems[1].Format = DXGI_FORMAT_R32G32B32A32_SINT;
    elems[1].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[1].InstanceDataStepRate = 1;
    elems[1].SemanticName = "POSITION";
    elems[1].SemanticIndex = 1;

    elems[2].AlignedByteOffset = sizeof(XMINT4);
    elems[2].InputSlot = 1;
    elems[2].Format = DXGI_FORMAT_R32G32B32A32_SINT;
    elems[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[2].InstanceDataStepRate = 1;
    elems[2].SemanticName = "POSITION";
    elems[2].SemanticIndex = 2;

    elems[3].AlignedByteOffset = 2 * sizeof(XMINT4);
    elems[3].InputSlot = 1;
    elems[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    elems[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[3].InstanceDataStepRate = 1;
    elems[3].SemanticName = "COLOR";

    elems[4].AlignedByteOffset = 2 * sizeof(XMINT4) + sizeof(XMFLOAT4);
    elems[4].InputSlot = 1;
    elems[4].Format = DXGI_FORMAT_R32_UINT;
    elems[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[4].InstanceDataStepRate = 1;
    elems[4].SemanticName = "TEXCOORD";
    elems[4].SemanticIndex = 0;

    Vertex2D vertices[] = 
    {
        { XMFLOAT2(-0.5f, -0.5f) },
        { XMFLOAT2(0.5f, -0.5f) },
        { XMFLOAT2(-0.5f, 0.5f) },
        { XMFLOAT2(0.5f, 0.5f) },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(vertices[0]);
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = vertices;
    init.SysMemPitch = bd.ByteWidth;

    CHECKHR(device->CreateBuffer(&bd, &init, &_vertexBuffer));

    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(PerFrame);
    bd.StructureByteStride = sizeof(PerFrame);

    CHECKHR(device->CreateBuffer(&bd, nullptr, &_constantBuffer));

    size_t length = 0;
    auto buffer = ReadFile("draw2d_vs.cso", &length);
    CHECKHR(device->CreateVertexShader(buffer.get(), length, nullptr, &_vertexShader));
    CHECKHR(device->CreateInputLayout(elems, _countof(elems), buffer.get(), length, &_inputLayout));

    buffer = ReadFile("draw2d_ps.cso", &length);
    CHECKHR(device->CreatePixelShader(buffer.get(), length, nullptr, &_pixelShader));

    D3D11_RASTERIZER_DESC rd = {};
    rd.CullMode = D3D11_CULL_BACK;
    rd.DepthClipEnable = TRUE;
    rd.FillMode = D3D11_FILL_SOLID;
    rd.ScissorEnable = TRUE;

    CHECKHR(device->CreateRasterizerState(&rd, &_rasterizer));

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

    CHECKHR(device->CreateBlendState(&blendDesc, &_blendState));
}

_Use_decl_annotations_
void Draw2D::Add(const Texture& texture, const RECT& source, const RECT& dest, const XMFLOAT4& color)
{
    auto batch = FindBatch(texture, _batches);
    batch->Instances[batch->NumInstances++] = Instance2D(source, dest, color, _numInstances++, texture.GetIndex());
}

_Use_decl_annotations_
void Draw2D::AddFullScreen(const Texture& texture, const RECT& source, const RECT& dest, const XMFLOAT4& color)
{
    auto batch = FindBatch(texture, _fullScreenBatches);
    batch->Instances[batch->NumInstances++] = Instance2D(source, dest, color, _numInstances++, texture.GetIndex(), true);
}

void Draw2D::Draw()
{
    auto lock = GetGraphics().LockContext();

    static const uint32_t strides[] = { sizeof(Vertex2D), sizeof(Instance2D) };
    static const uint32_t offsets[] = { 0, 0 };
    static const float blendFactors[] = { 1, 1, 1, 1 };

    _context->IASetInputLayout(_inputLayout.Get());
    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    _context->PSSetSamplers(0, 1, GetGraphics().GetPointWrapSampler().GetAddressOf());
    _context->RSSetState(_rasterizer.Get());
    _context->OMSetBlendState(_blendState.Get(), blendFactors, 0xFFFFFFFF);
    _context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), strides, offsets);
    _context->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

    // Create a scissor rect around virtual canvas.

    auto& config = GetConfig();

    D3D11_RECT canvasBounds;
    canvasBounds.left = ((int32_t)config.ScreenWidth - (int32_t)config.CanvasWidth) / 2;
    canvasBounds.top = ((int32_t)config.ScreenHeight - (int32_t)config.CanvasHeight) / 2;
    canvasBounds.right = canvasBounds.left + config.CanvasWidth;
    canvasBounds.bottom = canvasBounds.top + config.CanvasHeight;

    _context->RSSetScissorRects(1, &canvasBounds);

    _constants.CanvasOffset = XMFLOAT2((float)canvasBounds.left, (float)canvasBounds.top);
    _constants.InvScreenSize = XMFLOAT2(1.0f / config.ScreenWidth, 1.0f / config.ScreenHeight);
    _constants.TotalInstances = _numInstances;
    _context->UpdateSubresource(_constantBuffer.Get(), 0, nullptr, &_constants, sizeof(_constants), 0);

    uint32_t currentlyBoundPoolId = static_cast<uint32_t>(-1);
    for (auto& batch : _batches)
    {
        D3D11_BOX box = {};
        box.right = sizeof(Instance2D) * batch->NumInstances;
        box.bottom = 1;
        box.back = 1;

        _context->UpdateSubresource(batch->InstanceBuffer.Get(), 0, &box, batch->Instances.get(), box.right, 0);
        _context->IASetVertexBuffers(1, 1, batch->InstanceBuffer.GetAddressOf(), &strides[1], offsets);

        if (batch->Texture.GetPool()->GetId() != currentlyBoundPoolId)
        {
            _context->VSSetShaderResources(0, 1, batch->Texture.GetPool()->GetAddress());
            _context->PSSetShaderResources(0, 1, batch->Texture.GetPool()->GetAddress());
            currentlyBoundPoolId = batch->Texture.GetPool()->GetId();
        }

        _context->DrawInstanced(4, batch->NumInstances, 0, 0);

        // recycle the batch
        _recycle.push_back(batch);
    }

    _context->RSSetState(nullptr);
    for (auto& batch : _fullScreenBatches)
    {
        D3D11_BOX box = {};
        box.right = sizeof(Instance2D) * batch->NumInstances;
        box.bottom = 1;
        box.back = 1;

        _context->UpdateSubresource(batch->InstanceBuffer.Get(), 0, &box, batch->Instances.get(), box.right, 0);
        _context->IASetVertexBuffers(1, 1, batch->InstanceBuffer.GetAddressOf(), &strides[1], offsets);

        if (batch->Texture.GetPool()->GetId() != currentlyBoundPoolId)
        {
            _context->VSSetShaderResources(0, 1, batch->Texture.GetPool()->GetAddress());
            _context->PSSetShaderResources(0, 1, batch->Texture.GetPool()->GetAddress());
            currentlyBoundPoolId = batch->Texture.GetPool()->GetId();
        }

        _context->DrawInstanced(4, batch->NumInstances, 0, 0);

        // recycle the batch
        _recycle.push_back(batch);
    }

    // clear the batch list
    _batches.clear();
    _fullScreenBatches.clear();
    _numInstances = 0;

    _context->RSSetState(nullptr);
}

_Use_decl_annotations_
std::shared_ptr<Draw2D::Batch2D> Draw2D::FindBatch(const Texture& texture, std::vector<std::shared_ptr<Batch2D>>& batches)
{
    uint32_t id = texture.GetPool()->GetId();
    auto it = std::begin(batches);

    for (; it != std::end(batches); ++it)
    {
        uint32_t entryId = it->get()->Texture.GetPool()->GetId();
        if (entryId == id)
        {
            // Is there room in this batch?
            if (it->get()->NumInstances + 1 < _maxInstancesPerBatch)
            {
                return *it;
            }
        }
        else if (entryId > id)
        {
            // need to insert new batch here
            break;
        }
    }

    std::shared_ptr<Batch2D> batch;

    // Do we have one we can reuse?
    if (!_recycle.empty())
    {
        batch = _recycle.back();
        _recycle.pop_back();
    }
    else
    {
        // Need to create a new batch
        batch.reset(new Batch2D);
        batch->Instances.reset(new Instance2D[_maxInstancesPerBatch]);

        ComPtr<ID3D11Device> device;
        _context->GetDevice(&device);

        D3D11_BUFFER_DESC bd = {};
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.ByteWidth = _maxInstancesPerBatch * sizeof(Instance2D);
        bd.StructureByteStride = sizeof(Instance2D);
        bd.Usage = D3D11_USAGE_DEFAULT;

        CHECKHR(device->CreateBuffer(&bd, nullptr, &batch->InstanceBuffer));
    }

    batch->NumInstances = 0;
    batch->Texture = texture;

    batches.insert(it, batch);
    return batch;
}
