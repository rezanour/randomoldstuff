cbuffer ColorConstants
{
    float4 CeilingColor;
    float4 FloorColor;
};

cbuffer HeightConstants
{
    uint ViewTop;
    uint ViewHeight;
};

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    float4 colors[] = { CeilingColor, FloorColor };
    return colors[(position.y - ViewTop) * 2 /  ViewHeight];
}
