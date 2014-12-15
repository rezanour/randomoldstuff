/*
    Vertex Shader for Directional Lights.

    The input is in post-projection space, and must be returned
    back to view space. The ViewRay (ray from eye through the point) is
    then computed from that.
*/

cbuffer vsPerFrame
{
    float4x4 InvProjection;
};

struct vsDirectionalLightInput
{
    float4 Position : POSITION;
};

struct vsDirectionalLightOutput
{
    float4 Position : SV_POSITION;
    float3 ViewRay : POSITION;
};

vsDirectionalLightOutput main(vsDirectionalLightInput input)
{
    vsDirectionalLightOutput output;

    output.Position = input.Position;

    float4 viewPosition = mul(InvProjection, input.Position);
    viewPosition.xyz /= viewPosition.w;
    output.ViewRay = float3(viewPosition.xy / viewPosition.z, 1.0f);

    return output;
}
