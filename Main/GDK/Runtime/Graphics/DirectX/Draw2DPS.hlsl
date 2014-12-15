#include "ShaderCommon.hlsli"

float4 main(Output2D input) : SV_TARGET0
{
    return Texture.Sample(Sampler, input.TexCoord);
}
