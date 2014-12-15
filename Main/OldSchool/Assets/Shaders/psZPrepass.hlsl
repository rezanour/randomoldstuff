//==============================================================================
//  PixelShader for z prepass (no pixels should be drawn as no RT should be bound)
//==============================================================================

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    return float4(0, 0, 0, 0);
}
