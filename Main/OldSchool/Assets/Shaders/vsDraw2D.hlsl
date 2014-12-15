//==============================================================================
//  VertexShader for drawing 2D images in screen space
//
//  Initial version does not yet support instancing, as the texture atlasing
//  problem needs to be solved first to make that feasible in this game.
//
//==============================================================================

cbuffer vsPerFrame
{
    uint NumInstances;
};

struct vsDraw2DInput
{
    float2 Position : POSITION0;
    float2 TexCoord : TEXCOORD0;

    // Instance data
    float4 Source : TEXCOORD1;
    float4 Dest : TEXCOORD2;
    uint InstanceId: TEXCOORD3;
};

struct vsDraw2DOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

vsDraw2DOutput main(vsDraw2DInput input)
{
    vsDraw2DOutput output;

    float2 srcScale = float2(input.Source.z - input.Source.x, input.Source.w - input.Source.y);
    float2 dstScale = float2(input.Dest.z - input.Dest.x, input.Dest.w - input.Dest.y);

    output.Position = float4(input.Position * dstScale + input.Dest.xy, 0.99999f - input.InstanceId / (float)NumInstances, 1);
    output.Position.xy = output.Position.xy * float2(2, -2) + float2(-1, 1);
    output.TexCoord = input.TexCoord * srcScale + input.Source.xy;

    return output;
}
