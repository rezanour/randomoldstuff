//==============================================================================
//  VertexShader for debug drawing
//==============================================================================

cbuffer vsPerFrame
{
    float4x4 ViewProjection;
};

struct vsDebugDrawInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct vsDebugDrawOutput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

vsDebugDrawOutput main(vsDebugDrawInput input)
{
    vsDebugDrawOutput output;
    output.Position = mul(ViewProjection, float4(input.Position, 1));
    output.Color = input.Color;

    // Apply a very small z-bias so that debug rendering sits on top of equally
    // positioned geometry.
    output.Position.z -= 0.0001f;

    return output;
}
