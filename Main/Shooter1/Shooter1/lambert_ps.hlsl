//
// Pixel shader for simple lambertian matte rendering.
//

Texture2DArray Textures;
SamplerState Sampler;

struct PSInput
{
    float4                  Position    : SV_POSITION;
    float3                  WorldNormal : NORMAL0;
    float2                  TexCoord    : TEXCOORD0;
    nointerpolation uint    TexIndex    : TEXCOORD1;
};

float4 main(PSInput input) : SV_TARGET
{
    return Textures.Sample(Sampler, float3(input.TexCoord, input.TexIndex));
}
