struct VSOutput
{
    float4 Position : SV_POSITION;
    float4 ScreenPosition : POSITION;
};

VSOutput main(float2 pos : POSITION)
{
    VSOutput output;
    output.Position = float4(pos, 0.999999f, 1);
    output.ScreenPosition = output.Position * 0.5 + 0.5;
    return output;
}
