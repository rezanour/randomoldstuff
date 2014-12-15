/*
    Pixel Shader for Directional Lights.
*/

cbuffer psPerFrame : register(b0)
{
    float ProjectionA; // m33 of projection matrix
    float ProjectionB; // m43 of projection matrix
    float2 ScreenSize;
};

cbuffer psPerLight : register (b1)
{
    float4 Direction;
    float4 Color;
    float4x4 InvView;
    float4x4 LightView;
    float4x4 LightProjection;
};

Texture2D DepthTexture : register (t0);
Texture2D NormalTexture : register (t1);
Texture2D ShadowMapTexture : register (t2);

sampler PointSampler;
sampler LinearSampler;

struct psDirectionalLightInput
{
    float4 Position : SV_POSITION;
    float3 ViewRay : POSITION;
};

float4 main(psDirectionalLightInput input) : SV_TARGET
{
    // unpack from [0, 1] -> [-1, 1]
    float2 texCoord = input.Position.xy / ScreenSize;
    float3 normal = normalize(NormalTexture.Sample(PointSampler, texCoord).xyz * 2 - 1);

    float depth = DepthTexture.Sample(PointSampler, texCoord).x;
    float linearDepth = ProjectionB / (depth - ProjectionA);

    float3 viewPosition = input.ViewRay.xyz * linearDepth;

    float4 shadowPosition = mul(InvView, float4(viewPosition, 1));
    shadowPosition = mul(LightView, shadowPosition);
    shadowPosition = mul(LightProjection, shadowPosition);
    shadowPosition.xyz /= shadowPosition.w;
    shadowPosition.xy = shadowPosition.xy* float2(0.5, -0.5) + 0.5;

    float2 shadowDepth = ShadowMapTexture.Sample(LinearSampler, shadowPosition.xy).xy;
    float u = shadowDepth.x;
    float o2 = shadowDepth.y - (shadowDepth.x * shadowDepth.x);

    float uMinusDSqr = (u - shadowPosition.z);
    uMinusDSqr *= uMinusDSqr;
    float pMax = o2 / (o2 + uMinusDSqr);

    float nDotL = saturate(dot(normal, Direction.xyz));

    float3 h = normalize(normalize(-input.ViewRay.xyz) + Direction.xyz);
    float spec = pow(dot(normal, h), 250);

    if (shadowPosition.z < (shadowDepth.x + 0.001))
    {
        return float4(Color.xyz * nDotL + spec, 0);
    }
    else
    {
        return float4((Color.xyz * nDotL + spec) * pMax, 0);
    }
}
