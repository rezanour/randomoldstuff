/*
    Pixel Shader for GBuffer creation.

    The geometry buffers are designed to be in camera view space.
    This makes some of the algorithms slighty more complicated, but the result is much more efficient.
    It reduces the number of full transformations that need to be done, and simplifies inputs.
*/

struct psGBufferInput
{
    float4 Position : SV_POSITION;
    float3 ViewPosition : POSITION;
    float3 ViewNormal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct psGBufferOutput
{
    float4 Diffuse : SV_TARGET0;
    float4 ViewNormal : SV_TARGET1;
};

Texture2D NormalMap;
sampler PointSampler;

float3x3 ComputeTangentFrame(float3 normal, float3 position, float2 texCoord)
{
    float3 dxPosition = ddx(position);
    float3 dyPosition = ddy(position);

    float2 dxTexCoord = ddx(texCoord);
    float2 dyTexCoord = ddy(texCoord);

    float3 tangent = normalize(dyTexCoord.y * dxPosition - dxTexCoord.y * dyPosition);
    float3 bitangent = normalize(dyTexCoord.x * dxPosition - dxTexCoord.x * dyPosition);

    float3 x = cross(normal, tangent);
    tangent = normalize(cross(x, normal));

    x = cross(bitangent, normal);
    bitangent = normalize(cross(normal, x));

    return float3x3(tangent, bitangent, normal);
}

psGBufferOutput main(psGBufferInput input)
{
    psGBufferOutput output;

    output.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float3 normal = normalize(NormalMap.Sample(PointSampler, input.TexCoord) * 2 - 1);

    float3x3 tangentFrame = ComputeTangentFrame(normalize(input.ViewNormal), normalize(input.ViewPosition), input.TexCoord);
    output.ViewNormal = float4(normalize(mul(normal, tangentFrame)).xyz * 0.5 + 0.5, 1.0f);

    // transform normal from [-1, 1] range to [0, 1] range (for storage in texture)
    //output.ViewNormal = float4((normalize(input.ViewNormal) + 1) * 0.5f, 1.0);

    return output;
}
