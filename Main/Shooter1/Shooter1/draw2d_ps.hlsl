//
// Pixel shader for 2D rendering.
//

Texture2DArray Textures;
SamplerState Sampler;

struct PSInput
{
    float4                  Position : SV_POSITION;
    float4                  Color    : COLOR0;
    float2                  TexCoord : TEXCOORD0;
    nointerpolation uint    TexIndex : TEXCOORD1;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 color = Textures.Sample(Sampler, float3(input.TexCoord, input.TexIndex));
    clip(color.a - 0.125f); // discard pixels with < 0.125 alpha
    return color * input.Color;
}
