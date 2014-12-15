cbuffer Constants
{
    float4x4    View;
    float4x4    Projection;
    float2      Texture1SizeInTiles;
    float2      Texture2SizeInTiles;
};

struct Vertex
{
    float3  Position    : POSITION;
    float4  TexCoord    : TEXCOORD;
};

struct VertexOut
{
    float4  Position    : SV_POSITION;
    float4  TexCoord    : TEXCOORD;
};

VertexOut main(Vertex input)
{
    VertexOut output;
    output.Position = mul(Projection, mul(View, float4(input.Position, 1)));
    output.TexCoord = input.TexCoord;
    output.TexCoord.xy = output.TexCoord.xy * (input.TexCoord.z * rcp(Texture1SizeInTiles) + input.TexCoord.w * rcp(Texture2SizeInTiles));
    return output;
}
