/*
    Vertex Shader for Directional Light Shadow Map

    The input matrix is an orthographic projection
*/

cbuffer vsPerLight : register(b0)
{
    float4x4 View;
    float4x4 Projection;
};

cbuffer vsPerObject : register(b1)
{
    float4x4 World;
};

struct vsDirectionalShadowInput
{
    float4 Position : POSITION;
};

struct vsDirectionalShadowOutput
{
    float4 Position : SV_POSITION;
    float LinearDepth : TEXCOORD;
};

vsDirectionalShadowOutput main(vsDirectionalShadowInput input)
{
    vsDirectionalShadowOutput output;

    float4 viewPosition = mul(View, mul(World, input.Position));
    output.Position = mul(Projection, viewPosition);
    output.LinearDepth = output.Position.z;

    return output;
}
