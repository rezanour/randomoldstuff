cbuffer Constants
{
    float4x4  View;
    float4x4  Projection;
};

struct VSInput
{
    float3  Position : POSITION;
    float3  Normal : NORMAL;
    float2  TexCoord : TEXCOORD0;
    uint    TexIndex : TEXCOORD1;
    uint    CellIndex : TEXCOORD2;
};

struct VSOutput
{
    float4                  Position : SV_POSITION;
    float3                  Normal   : NORMAL;
    float2                  TexCoord : TEXCOORD0;
    nointerpolation uint    TexIndex : TEXCOORD1;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    const uint MapSize = 64;
    const float WorldScale = 1;

    float2 center = MapSize * WorldScale * 0.5f;
    float x = (input.CellIndex % MapSize) - center.x;
    float y = center.y - (input.CellIndex / MapSize);
    output.Position = mul(Projection, mul(View, float4(x + input.Position.x, input.Position.y, y + input.Position.z, 1)));

    output.TexCoord = input.TexCoord;
    output.Normal = input.Normal;
    output.TexIndex = input.TexIndex;

    //
    // Add fake lighting by picking other version of texture for East/West walls.
    // NOTE: only the first half or so of the textures appear to have alternate colors...
    //
    if (output.TexIndex < 50 && input.Normal.x != 0)
    {
        output.TexIndex = output.TexIndex - 1;
    }

    return output;
}
