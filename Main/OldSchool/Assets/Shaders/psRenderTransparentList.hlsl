//==============================================================================
//  PixelShader for rendering per-pixel linked list for 
//      order independent transparency (OIT)
//==============================================================================

#include "common.hlsli"

cbuffer psPerFrame
{
    uint ScreenWidth;
};

StructuredBuffer<OITNode> NodesSRV;
Buffer<uint> StartOffsetsSRV;
static const uint EndNode = 0xFFFFFFFF;

Texture2D Background;

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    int3 pos = GetPixelCoord(position);
    uint offsetAddress = ScreenWidth * pos.y + pos.x;

    // Get first node for pixel
    uint nodeIndex = StartOffsetsSRV.Load(offsetAddress);

    OITNode samples[20];
    uint count = 0;
    uint i, j;

    // Walk list into local array for sorting and rendering. Use insertion sort
    // to build the array now
    while (nodeIndex != EndNode)
    {
        OITNode node = NodesSRV[nodeIndex];
        for (i = 0; i < count; ++i)
        {
            if (node.Depth > samples[i].Depth)
            {
                for (j = i; j < count; ++j)
                {
                    samples[j + 1] = samples[j];
                }
                break;
            }
        }
        samples[i] = node;
        ++count;

        nodeIndex = node.Next;
    }

    float4 color = float4(Background.Load(pos).xyz, 1);
    for (i = 0; i < count; ++i)
    {
        float4 s = UnpackColor(samples[i].Color);
        color.xyz = s.xyz + (color.xyz * (1 - s.w));
    }

    return color;
}
