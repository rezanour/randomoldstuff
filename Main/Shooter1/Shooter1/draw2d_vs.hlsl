//
// Vertex shader for 2D rendering.
//

Texture2DArray Textures;

cbuffer PerFrame
{
    // In pixels
    float2 InvScreenSize;

    // In pixels
    float2 CanvasOffset;

    uint TotalInstances;
};

struct VSInput
{
    //
    // Per Vertex:
    //

    // VertexPosition is in relative normalized quad corners.
    float2 Offset : POSITION0;

    //
    // Per Instance:
    //

    // Destination rect
    int4 DestRect : POSITION1;
    // Source rect
    int4 SourceRect : POSITION2;
    // Tinting color
    float4 Color : COLOR0;
    // Index used to compute draw order/depth, and Texture index in array. Packed into single uint.
    // If high bit of index is set, ignore transform
    uint IndexAndTexIndex : TEXCOORD0;
};

struct VSOutput
{
    float4                  Position : SV_POSITION;
    float4                  Color    : COLOR;
    float2                  TexCoord : TEXCOORD0;
    nointerpolation uint    TexIndex : TEXCOORD1;
};

//
// Given a packed rectangle, inverse target size, and a normalized offset,
// convert to a normalized rectangle in the target space
//
float2 ConvertPackedRectToNormalized(int4 rect, float2 invTargetSize, float2 targetOffset, float2 normalizedOffset)
{
    float2 center = float2((rect.x + rect.z) * 0.5f, (rect.y + rect.w) * 0.5f);
    float2 size = float2(rect.z - rect.x, rect.w - rect.y);
    return (center + targetOffset + (normalizedOffset * size)) * invTargetSize;
}

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;

    bool UseTransform = true;

    if (input.IndexAndTexIndex & 0x80000000)
    {
        UseTransform = false;
        input.IndexAndTexIndex &= 0x7FFFFFFF;
    }

    // Unpack inputs
    uint InstanceNum = input.IndexAndTexIndex >> 16;
    uint TexIndex = input.IndexAndTexIndex & 0x0000ffff;


    // Retrieve texture size
    uint width, height, numElems, numMips;
    Textures.GetDimensions(0, width, height, numElems, numMips);

    float2 invTexSize = 1 / float2(width, height);

    // Construct the output position in post-projection space
    float2 pos;
    if (UseTransform)
    {
        pos = ConvertPackedRectToNormalized(input.DestRect, InvScreenSize, CanvasOffset, input.Offset);
    }
    else
    {
        pos = ConvertPackedRectToNormalized(input.DestRect, InvScreenSize, 0, input.Offset);
    }

    pos = float2(pos.x, 1 - pos.y) * 2 - 1;

    // Compute z based on index values. Add 1 to denominator so that we don't get z of 1.0f (which will get dropped)
    output.Position = float4(pos, (TotalInstances - InstanceNum) / (float)(TotalInstances + 1), 1.0f);

    output.Color = input.Color;
    output.TexCoord = ConvertPackedRectToNormalized(input.SourceRect, invTexSize, 0, input.Offset);
    output.TexIndex = TexIndex;

    return output;
}
