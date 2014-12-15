//==============================================================================
//  PixelShader for building per-pixel linked list for 
//      order independent transparency (OIT)
//==============================================================================

#include "common.hlsli"

cbuffer psPerFrame
{
    uint ScreenWidth;
};

RWStructuredBuffer<OITNode> Nodes;
RWByteAddressBuffer StartOffsets;

struct psInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

[earlydepthstencil]
float4 main(psInput input) : SV_TARGET
{
    // Get index of next available node
    uint nodePosition = Nodes.IncrementCounter();

    // Determine which offset goes with this pixel
    // and swap it to the new position value, getting old one
    int3 pos = GetPixelCoord(input.Position);

    // location is in bytes, so we need to compute that from the pixel location
    uint pixelLocation = 4 * ((ScreenWidth * pos.y) + pos.x);

    uint oldNodePos;
    StartOffsets.InterlockedExchange(pixelLocation, nodePosition, oldNodePos);

    // Add in the new node
    OITNode node;
    node.Color = PackColor(input.Color);
    node.Depth = input.Position.z;
    node.Next = oldNodePos;
    Nodes[nodePosition] = node;

    return Nodes[nodePosition].Color;
}
