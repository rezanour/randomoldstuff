/*
    Vertex Shader for Point Lights. Transforms light volume, and computes ViewRay
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

struct vsPointLightInput
{
    float3 Position : POSITION;
};

struct vsPointLightOutput
{
    float4 Position : SV_POSITION;
    float3 ViewRay : POSITION;
};

vsPointLightOutput main(vsPointLightInput input)
{
    vsPointLightOutput output;

    float4 viewPosition = mul(View, mul(World, float4(input.Position, 1)));
    output.Position = mul(Projection, viewPosition);
    viewPosition.xyz /= viewPosition.w;
    output.ViewRay = float3(viewPosition.xy / viewPosition.z, 1.0f);

    return output;
}
