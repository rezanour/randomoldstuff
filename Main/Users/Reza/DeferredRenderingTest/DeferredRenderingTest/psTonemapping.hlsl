/*
    Pixel Shader for Tonemapping
*/

cbuffer psPerFrame : register(b0)
{
    float2 ScreenSize;
    float AverageLuminance;
    float MaxLuminance;
};

static const float BrightnessThreshold = 0.15f;
static const float MiddleGray = 0.5f;
static const float LuminanceWhite = 1.0f;

Texture2D SourceImage : register(t0);
sampler LinearSampler : register(s0);

static const float3x3 RGB2YCbCr =
float3x3(
    float3(0.299, 0.587, 0.114),
    float3(-0.169, -0.331, 0.500),
    float3(0.500, -0.419, -0.081)
);

static const float3 ConversionOffset = float3(0, 0.5, 0.5);

static const float3x3 YCbCr2RGB =
float3x3(
    float3(1.000, 0.000, 1.400),
    float3(1.000, -0.343, -0.711),
    float3(1.000, 1.765, 0.000)
);

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    float2 texCoord = position.xy / ScreenSize;
    float3 sample = SourceImage.Sample(LinearSampler, texCoord).xyz;

    float3 ycbcr = mul(RGB2YCbCr, sample);
    ycbcr += ConversionOffset;

    float L = 0.1 * ycbcr.x / AverageLuminance;
    ycbcr.x = L * (1 + L / (MaxLuminance * MaxLuminance)) / (1 + L);

    ycbcr -= ConversionOffset;
    float3 rgb = mul(YCbCr2RGB, ycbcr);

    return float4(rgb, 1);
}
