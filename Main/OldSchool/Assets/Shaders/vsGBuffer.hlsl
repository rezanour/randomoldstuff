//==============================================================================
//  VertexShader for rendering geometry into the GBuffer
//==============================================================================

cbuffer vsPerFrame
{
    float4x4 View;
    float4x4 Projection;
};

cbuffer vsPerObject
{
    float4x4 World;
};

struct vsGBufferInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD;
};

struct vsGBufferOutput
{
    float4 Position : SV_POSITION;
    float3 ViewPosition : POSITION;
    float3 ViewNormal : NORMAL;
    float3 ViewTangent : TANGENT;
    float3 ViewBitangent : BITANGENT;
    float2 TexCoord : TEXCOORD;
};

vsGBufferOutput main(vsGBufferInput input)
{
    vsGBufferOutput output;

    float4x4 wv = mul(View, World);

    // only valid for linear affine transforms (no shearing, no projection, etc... just plain position, rotation, scale)
    float3x3 invTransWV = (float3x3)wv;

    float4 viewPosition = mul(wv, float4(input.Position, 1));
    output.Position = mul(Projection, viewPosition);
    output.ViewPosition = viewPosition.xyz;
    output.ViewNormal = mul(invTransWV, input.Normal);
    output.ViewTangent = mul(invTransWV, input.Tangent);
    output.ViewBitangent = mul(invTransWV, cross(input.Normal, input.Tangent));
    output.TexCoord = input.TexCoord;

    return output;
}
