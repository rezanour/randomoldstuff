/*
    Vertex Shader for GBuffer creation.

    The geometry buffers are designed to be in camera view space.
    This makes some of the algorithms slighty more complicated, but the result is much more efficient.
    It reduces the number of full transformations that need to be done, and simplifies inputs.
*/

// These are only set once per frame, not on each draw call
cbuffer PerFrame
{
    float4x4 View;
    float4x4 Projection;
};

// These are set per draw call as needed
cbuffer PerObject
{
    float4x4 World;
};

struct vsGBufferInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct vsGBufferOutput
{
    float4 Position : SV_POSITION;
    float3 ViewPosition : POSITION;
    float3 ViewNormal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

vsGBufferOutput main(vsGBufferInput input)
{
    vsGBufferOutput output;

    float4x4 worldView = mul(View, World);

    float4 viewPosition = mul(worldView, float4(input.Position, 1));
    output.Position = mul(Projection, viewPosition);
    output.ViewPosition = viewPosition.xyz / viewPosition.w;
    output.ViewNormal = mul((float3x3)worldView, input.Normal);
    output.TexCoord = input.TexCoord;

    return output;
}
