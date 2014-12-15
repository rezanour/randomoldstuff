/*
    Pixel Shader for Point Lights.
*/

cbuffer psPerFrame : register(b0)
{
    float ProjectionA; // m33 of projection matrix
    float ProjectionB; // m43 of projection matrix
    float2 ScreenSize;
};

cbuffer psPerLight : register (b1)
{
    float4x4 InvView;
    float4 Position;
    float4 Color;
    float Radius;
    float ParaboloidNear;
};

Texture2D DepthTexture : register (t0);
Texture2D NormalTexture : register (t1);
Texture2D ShadowFront : register (t2);
Texture2D ShadowBack : register (t3);

sampler PointSampler;
sampler LinearSampler;

struct psPointLightInput
{
    float4 Position : SV_POSITION;
    float3 ViewRay : POSITION;
};

float4 main(psPointLightInput input) : SV_TARGET
{
    // unpack from [0, 1] -> [-1, 1]
    float2 texCoord = input.Position.xy / ScreenSize;
    float3 normal = normalize(NormalTexture.Sample(PointSampler, texCoord).xyz * 2 - 1);

    float depth = DepthTexture.Sample(PointSampler, texCoord).x;
    float linearDepth = ProjectionB / (depth - ProjectionA);

    float3 viewPosition = input.ViewRay.xyz * linearDepth;

    // view space vector from surface to point light
    float3 L = Position.xyz - viewPosition;
    float attenuation = saturate(1.0 - length(L) / Radius);
    L = normalize(L);

    float nDotL = saturate(dot(normal, L));

    float3 shadowPosition = mul(InvView, float4(viewPosition, 1)).xyz;
    shadowPosition -= mul(InvView, Position).xyz;

    float len = length(shadowPosition);
    float3 p0 = shadowPosition / len;
    float alpha = 0.5f + shadowPosition.z / Radius;

    // front tex coord
    p0.z = p0.z + 1;
    p0.x = (p0.x / p0.z) * 0.5 + 0.5;
    p0.y = (p0.y / p0.z) * -0.5 + 0.5;
    p0.z = len / Radius;

    float3 p1 = shadowPosition / len;

    // back tex coord
    p1.z = 1 - p1.z;
    p1.x = (p1.x / p1.z) * -0.5 + 0.5;
    p1.y = (p1.y / p1.z) * -0.5 + 0.5;
    p1.z = len / Radius;

    float2 shadowDepth;
    float myDepth;
    if (alpha >= 0.5f)
    {
        shadowDepth = ShadowFront.Sample(LinearSampler, p0.xy).xy;
        myDepth = p0.z;
    }
    else
    {
        shadowDepth = ShadowBack.Sample(LinearSampler, p1.xy).xy;
        myDepth = p1.z;
    }

    //float u = shadowDepth.x;
    //float o2 = shadowDepth.y - (shadowDepth.x * shadowDepth.x);

    //float uMinusDSqr = (u - myDepth);
    //uMinusDSqr *= uMinusDSqr;
    //float pMax = o2 / (o2 + uMinusDSqr);

    if (myDepth < shadowDepth.x + 0.001f)
    {
        return float4(Color.xyz * nDotL * attenuation, 0);
    }
    else
    {
        return float4(Color.xyz * nDotL * attenuation * 0, 0);
    }
}
