//==============================================================================
//  PixelShader for rendering ambient lights
//==============================================================================

#include <common.hlsli>

cbuffer psPerDraw
{
    float3 LightColor;
};

Texture2D Diffuse;

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    int3 pixelCoord = GetPixelCoord(position);
    float3 diffuse = Diffuse.Load(pixelCoord).rgb;
    return float4(diffuse * LightColor, 0);
}
