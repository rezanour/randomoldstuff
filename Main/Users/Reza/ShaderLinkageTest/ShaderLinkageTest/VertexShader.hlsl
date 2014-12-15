cbuffer Constants
{
    float4x4 World;
    float4x4 ViewProjection;
};

struct INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct OUTPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
};

OUTPUT main(INPUT input)
{
    OUTPUT output;

    float4 worldPosition = mul(World, float4(input.Position, 1));
    output.Position = mul(ViewProjection, worldPosition);
    output.WorldPosition = worldPosition.xyz / worldPosition.w;
    output.Normal = mul((float3x3)World, input.Normal);

    return output;
}
