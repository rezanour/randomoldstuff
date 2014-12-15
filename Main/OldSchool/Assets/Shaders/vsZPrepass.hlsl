//==============================================================================
//  VertexShader for rendering geometry into the depth buffer for z-prepass
//==============================================================================

cbuffer vsPerFrame
{
    float4x4 View;
    float4x4 Projection;
};

cbuffer vsPerObject
{
    float4x4 World;
};

float4 main(float3 position : POSITION) : SV_POSITION
{
    float4 pos = mul(Projection, mul(View, mul(World, float4(position, 1))));
    pos.z += 0.0001f;
    return pos;
}
