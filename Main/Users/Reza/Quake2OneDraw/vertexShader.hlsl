cbuffer VSPerFrame
{
    float4x4 View;
    float4x4 Projection;
};

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    uint TexIndex : TEXCOORD1;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    nointerpolation uint TexIndex : TEXCOORD1;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;

    output.Position = mul(Projection, mul(View, float4(input.Position, 1)));
    output.Normal = input.Normal;
    output.TexCoord = input.TexCoord;
    output.TexIndex = input.TexIndex;

    return output;
}
