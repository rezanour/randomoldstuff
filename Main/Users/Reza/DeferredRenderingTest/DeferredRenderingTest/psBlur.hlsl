/*
    Pixel Shader for Simple Gaussian Blur.
*/

static const int2 SamplePoints[7] =
{
    { -3, 0 },
    { -2, 0 },
    { -1, 0 },
    { 0, 0 },
    { 1, 0 },
    { 2, 0 },
    { 3, 0 },
};

static const float SampleWeights[7] =
{
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
};

cbuffer psConstants
{
    float2 ScreenSize;
    float FlipXY;
};

Texture2D SourceImage;
sampler LinearSampler;

float4 main(float4 input : SV_POSITION) : SV_TARGET
{
    float4 color = 0;

    float2 texCoord = input.xy / ScreenSize;

    for (int i = 0; i < 7; ++i)
    {
        float2 offset = FlipXY ? SamplePoints[i].yx : SamplePoints[i].xy;
        offset /= ScreenSize;
        color += SourceImage.Sample(LinearSampler, texCoord + offset) * SampleWeights[i];
    }

    return color;
}
