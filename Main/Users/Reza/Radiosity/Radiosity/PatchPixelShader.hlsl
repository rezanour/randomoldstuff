Texture2D <float> LightMap;
sampler Sampler;

struct INPUT
{
    float4 Position : SV_POSITION;
    float2 LightUV : TEXCOORD;
};

float main(INPUT input) : SV_TARGET
{
    // TODO: make the width/height constants that are set from code
    return LightMap.Sample(Sampler, input.LightUV);
}
