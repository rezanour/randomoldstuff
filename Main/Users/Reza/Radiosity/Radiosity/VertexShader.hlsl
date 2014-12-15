cbuffer Constants
{
    float4x4 World;
    float4x4 ViewProjection;
};

struct INPUT
{
    float3 Position : POSITION;
    float2 LightUV : TEXCOORD0;
    float2 DiffuseUV : TEXCOORD1;
};

struct OUTPUT
{
    float4 Position : SV_POSITION;
    float2 LightUV : TEXCOORD0;
    float2 DiffuseUV : TEXCOORD1;
};

OUTPUT main(INPUT input)
{
    OUTPUT output;

    output.Position = mul(ViewProjection, mul(World, float4(input.Position, 1)));
    output.LightUV = input.LightUV;
    output.DiffuseUV = input.DiffuseUV;

    return output;
}
