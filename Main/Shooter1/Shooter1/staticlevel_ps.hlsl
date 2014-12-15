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
    nointerpolation uint    TexIndexAndStartMip    : TEXCOORD1;
};

float4 main(PSInput input) : SV_TARGET
{
    uint texIndex = input.TexIndexAndStartMip >> 16;
    uint startMip = input.TexIndexAndStartMip & 0x0000FFFF;

    return Textures.SampleBias(Sampler, float3(input.TexCoord, texIndex), ((float)startMip / 100.0f));
}
