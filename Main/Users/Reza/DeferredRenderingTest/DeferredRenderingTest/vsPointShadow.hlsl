/*
    Vertex Shader for Point light shadow maps

    This is achieved using dual-paraboloid mapping
*/

cbuffer vsPerLight
{
    float4x4 View;
    float ParaboloidNear;
    float ParaboloidFar;
};

cbuffer vsPerObject
{
    float4x4 World;
};

struct vsPointShadowInput
{
    float4 Position : POSITION;
};

struct vsPointShadowOutput
{
    float4 Position : SV_POSITION;
    float LinearDepth : TEXCOORD;
};

vsPointShadowOutput main(vsPointShadowInput input)
{
    vsPointShadowOutput output;

    // transform into view space
    output.Position = mul(View, mul(World, input.Position));

    // store len & normalize
    float len = length(output.Position.xyz);
    output.Position /= len;

    // map z to either front or back, then divide through xy
    // to get paraboloid mapped position
    output.Position.z += 1;
    output.Position.x /= output.Position.z;
    output.Position.y /= output.Position.z;

    // compute true z in paraboloid space
    output.Position.z = (len - ParaboloidNear) / (ParaboloidFar - ParaboloidNear);
    output.Position.w = 1;

    output.LinearDepth = output.Position.z;

    return output;
}
