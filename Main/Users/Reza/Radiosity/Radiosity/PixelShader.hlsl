Texture2D <float> LightMap;
Texture2D <float4> Diffuse;
sampler Sampler;

struct INPUT
{
    float4 Position : SV_POSITION;
    float2 LightUV : TEXCOORD0;
    float2 DiffuseUV : TEXCOORD1;
};

float4 main(INPUT input) : SV_TARGET
{
    float light = LightMap.Sample(Sampler, input.LightUV);
    float4 diffuse = Diffuse.Sample(Sampler, input.DiffuseUV);

    return diffuse * light;
}
