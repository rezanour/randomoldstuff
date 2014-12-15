/*
    Pixel Shader for High Pass Filter
*/

cbuffer psPerFrame
{
    float2 ScreenSize;
};

Texture2D SourceTexture;

sampler LinearSampler;

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    float2 texCoord = position.xy / ScreenSize;
    float4 source = SourceTexture.Sample(LinearSampler, texCoord);

    if (any(source.xyz - 1))
    {
        return source;
    }
    else
    {
        return float4(0, 0, 0, 1);
    }
}
