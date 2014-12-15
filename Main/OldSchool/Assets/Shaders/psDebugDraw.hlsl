//==============================================================================
//  PixelShader for debug drawing
//==============================================================================

cbuffer psPerFrame
{
    float DepthLayer;
};

struct psDebugDrawInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(psDebugDrawInput input) : SV_TARGET
{
    clip(input.Position.z - DepthLayer + 0.00001f);
    return input.Color;
}
