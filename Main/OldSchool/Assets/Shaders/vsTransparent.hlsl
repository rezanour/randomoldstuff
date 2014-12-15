//==============================================================================
//  VertexShader for building transparent object list
//==============================================================================

cbuffer vsPerFrame
{
    float4x4 ViewProjection;
};

cbuffer vsPerDraw
{
    float4x4 World;
};

struct vsTransparentInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct vsTransparentOutput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

vsTransparentOutput main(vsTransparentInput input)
{
    vsTransparentOutput output;
    output.Position = mul(ViewProjection, mul(World, float4(input.Position, 1)));
    output.Color = input.Color;
    return output;
}
