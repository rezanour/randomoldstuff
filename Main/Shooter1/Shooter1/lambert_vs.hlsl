//
// Vertex shader for simple lambertian matte rendering.
//

cbuffer PerFrame
{
    float4x4    View;
    float4x4    Projection;
};

cbuffer PerObject
{
    float4x4    World;
    uint        TexIndex;
};

struct VSInput
{
    float3 Position : POSITION0;
    float3 Normal   : NORMAL0;
    float2 TexCoord : TEXCOORD0;
};

struct VSOutput
{
    float4                  Position    : SV_POSITION;
    float3                  WorldNormal : NORMAL0;
    float2                  TexCoord    : TEXCOORD0;
    nointerpolation uint    TexIndex    : TEXCOORD1;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;

    // optimization for simple, affine, orthogonal matrices
    float3x3 invTransWorld = (float3x3)World;

    output.Position = mul(Projection, mul(View, mul(World, float4(input.Position, 1))));
    output.WorldNormal = mul(invTransWorld, input.Normal);
    output.TexCoord = input.TexCoord;
    output.TexIndex = TexIndex;

    return output;
}
