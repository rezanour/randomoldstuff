cbuffer Constants
{
    float4 CeilingColor;
    float4 FloorColor;
};

float4 main(float4 Position : SV_POSITION, float4 ScreenPosition : POSITION) : SV_TARGET
{
    return (ScreenPosition.y > 0.5f) ? CeilingColor : FloorColor;
}
