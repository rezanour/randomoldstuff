cbuffer FrameConstants
{
    float4x4 View;
    float4x4 Projection;
};

cbuffer ObjectConstants
{
    float4x4 World;
};

struct Vertex
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float2 TexCoord : TEXCOORD;
};

VertexOut main(Vertex input)
{
    VertexOut output;
    output.WorldPosition = mul(World, float4(input.Position, 1)).xyz;
    output.Position = mul(Projection, mul(View, float4(output.WorldPosition, 1)));
    output.Normal = mul((float3x3)World, input.Normal);
    output.Tangent = mul((float3x3)World, input.Tangent);
    output.Bitangent = cross(output.Normal, output.Tangent);
    output.TexCoord = input.TexCoord;
    return output;
}
