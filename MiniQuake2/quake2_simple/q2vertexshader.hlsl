#include "q2shadercommon.hlsli"

cbuffer VSGlobals
{
    float4x4 World;
    float4x4 InvTransWorld;
    float4x4 ViewProjection;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
//    int TexIndex : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(World, float4(input.Position, 1));
    output.Position = mul(ViewProjection, output.Position);
    output.Normal = mul(InvTransWorld, float4(input.Normal, 0)).xyz;
    output.UV = input.UV;
    return output;
}
