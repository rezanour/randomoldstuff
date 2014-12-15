//==============================================================================
//  VertexShader for debug rendering of the bsp
//==============================================================================

cbuffer vsPerFrame
{
    float4x4 View;
    float4x4 Projection;
};

float4 main(float3 position : POSITION) : SV_POSITION
{
    return mul(Projection, mul(View, float4(position, 1)));
}
