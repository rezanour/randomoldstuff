/*
    Pixel Shader for converting HDR lighting to Luminance
*/

cbuffer psPerFrame
{
    float2 ScreenSize;
};

Texture2D SourceTexture;
sampler LinearSampler;

static const float3x3 RGB2YUV =
float3x3(
    float3(0.299, 0.587, 0.114),
    float3(-0.147, -0.289, 0.436),
    float3(0.615, -0.515, -0.100)
);


float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    float2 texCoord = position.xy / ScreenSize;
    float4 sample = SourceTexture.Sample(LinearSampler, texCoord);

    float Y = 0.27 * sample.x + 0.67 * sample.y + 0.06 * sample.z;
    Y = max(0.00001, Y);
    return float4(Y, Y, Y, 1);
}
