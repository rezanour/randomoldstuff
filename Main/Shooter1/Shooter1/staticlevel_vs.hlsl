//
// Vertex shader for static level rendering.
//

cbuffer PerFrame
{
    float4x4    View;
    float4x4    Projection;
};

struct MaterialEntry
{
    uint Index;
    uint MipBias;
};

Texture2D<uint2> MaterialMap;

struct VSInput
{
    float3 Position : POSITION0;
    float3 Normal   : NORMAL0;
    float2 TexCoord : TEXCOORD0;
    uint   MaterialId : TEXCOORD1;
};

struct VSOutput
{
    float4                  Position    : SV_POSITION;
    float3                  WorldNormal : NORMAL0;
    float2                  TexCoord    : TEXCOORD0;
    nointerpolation uint    TexIndexAndMipBias : TEXCOORD1;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;

    output.Position = mul(Projection, mul(View, float4(input.Position, 1)));
    output.WorldNormal = input.Normal;
    output.TexCoord = input.TexCoord;

    MaterialEntry entry = (MaterialEntry)MaterialMap.Load(int3(input.MaterialId, 0, 0));
    output.TexIndexAndMipBias = (entry.Index << 16) | (entry.MipBias);

    return output;
}
