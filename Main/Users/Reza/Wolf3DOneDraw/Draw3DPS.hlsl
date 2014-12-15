struct PSInput
{
    float4                  Position : SV_POSITION;
    float3                  Normal   : NORMAL;
    float2                  TexCoord : TEXCOORD0;
    nointerpolation uint    TexIndex : TEXCOORD1;
};

Texture2DArray Textures;
sampler Sampler;

float4 main(PSInput input) : SV_TARGET
{
    return Textures.Sample(Sampler, float3(input.TexCoord, (float)input.TexIndex));
}
