/*
    Pixel Shader for Point Light Shadow Map.
*/

struct psPointShadowInput
{
    float4 Position : SV_POSITION;
    float LinearDepth : TEXCOORD;
};

float4 main(psPointShadowInput input) : SV_TARGET
{
    return float4(input.LinearDepth, input.LinearDepth * input.LinearDepth, 0, 0);
}
