cbuffer Constants
{
    float4x4 View;
    float4x4 Projection;
};

struct VSInput
{
    float2  Position        : POSITION0;
    float2  TexCoord        : TEXCOORD0;

    // Instance
    float2  WorldPosition   : POSITION1;
    uint    TexIndex        : TEXCOORD1;
};

struct VSOutput
{
    float4                  Position : SV_POSITION;
    float2                  TexCoord : TEXCOORD0;
    nointerpolation uint    TexIndex : TEXCOORD1;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    float4 position = float4(input.WorldPosition.x, 0, input.WorldPosition.y, 1);
    position = mul(View, position);

    //float scale = 1.0f / position.z;
    position.x += input.Position.x;
    position.y += input.Position.y;

    position = mul(Projection, position);

    output.Position = position;
    output.TexCoord = input.TexCoord;
    output.TexIndex = input.TexIndex;

    return output;
}
