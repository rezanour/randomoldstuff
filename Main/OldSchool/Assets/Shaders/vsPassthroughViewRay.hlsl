//==============================================================================
//  VertexShader for rendering post-projection quads, constructing the view ray to
//  pass along with the output.
//==============================================================================

#include "common.hlsli"

cbuffer vsPerFrame
{
    float4x4 InvProjection;
};

struct vsPassthroughViewRayInput
{
    float4 Position : POSITION;
};

struct vsPassthroughViewRayOutput
{
    float4 Position : SV_POSITION;
    float3 ViewRay : POSITION;
};

vsPassthroughViewRayOutput main(vsPassthroughViewRayInput input)
{
    vsPassthroughViewRayOutput output;

    output.Position = input.Position;

    float4 viewPosition = mul(InvProjection, input.Position);
    output.ViewRay = GetViewRay(viewPosition.xyz / viewPosition.w);

    return output;
}
