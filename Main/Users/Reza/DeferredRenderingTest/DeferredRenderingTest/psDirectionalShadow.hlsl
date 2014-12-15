/*
    Pixel Shader for Directional Light Shadow Map.
*/

struct psDirectionalShadowInput
{
    float4 Position : SV_POSITION;
    float LinearDepth : TEXCOORD;
};

float4 main(psDirectionalShadowInput input) : SV_TARGET
{
    return float4(input.LinearDepth, input.LinearDepth * input.LinearDepth, 0, 0);
}
