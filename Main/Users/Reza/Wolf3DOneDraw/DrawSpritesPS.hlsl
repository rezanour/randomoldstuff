struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    nointerpolation uint TexIndex : TEXCOORD1;
};

Texture2DArray Textures;
sampler Sampler;

float4 main(PSInput input) : SV_TARGET
{
    float4 sample = Textures.Sample(Sampler, float3(input.TexCoord, (float)input.TexIndex));
    clip(sample.a - 0.25f);
    return sample;
}
