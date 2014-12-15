//==============================================================================
//  VertexShader for Point Lights.
//==============================================================================

#include "common.hlsli"

cbuffer vsPerFrame
{
    float4x4 View;
    float4x4 Projection;
};

struct vsPointLightInput
{
    float3 Position : POSITION0;

    // Instance Data
    float3 Color : COLOR;
    float3 LightPosition : POSITION1;
    float Radius : TEXCOORD;
};

struct vsPointLightOutput
{
    float4 Position : SV_POSITION;

    float3 ViewPosition : POSITION0;
    float3 ViewLightPosition : POSITION1;
    float3 ViewRay : NORMAL;
    float3 Color : COLOR;
    float Radius : TEXCOORD;
};

vsPointLightOutput main(vsPointLightInput input)
{
    vsPointLightOutput output;

    float3 lightViewPosition = mul(View, float4(input.LightPosition, 1)).xyz;

    output.ViewLightPosition = lightViewPosition;
    output.Color = input.Color;
    output.Radius = input.Radius;

    // transform quad to the LightViewPosition and scale out by radius
    output.ViewPosition = lightViewPosition + input.Position * float3(input.Radius, input.Radius, 1);
    output.ViewRay = GetViewRay(output.ViewPosition);

    // adjust the quad now to back it up along view ray by radius
    output.ViewPosition += output.ViewRay * input.Radius;

    output.Position = mul(Projection, float4(output.ViewPosition, 1));

    return output;
}
