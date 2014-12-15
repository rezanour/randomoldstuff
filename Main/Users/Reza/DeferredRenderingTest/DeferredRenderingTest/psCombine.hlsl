/*
    Pixel Shader for Combine
*/

cbuffer psPerFrame
{
    float2 ScreenSize;
    uint Type;
};

Texture2D Texture1;
Texture2D Texture2;

sampler LinearSampler;

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    float2 texCoord = position.xy / ScreenSize;
    float4 sample1 = Texture1.Sample(LinearSampler, texCoord);
    float4 sample2 = Texture2.Sample(LinearSampler, texCoord);

    if (Type == 0)
    {
        return float4(sample1.xyz + sample2.xyz, 1);
    }
    else
    {
        return float4(sample1.xyz * sample2.xyz, 1);
    }
}
