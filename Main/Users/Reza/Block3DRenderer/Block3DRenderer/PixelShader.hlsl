Texture2D Texture[2];
sampler Sampler;

struct VertexOut
{
    float4  Position    : SV_POSITION;
    float4  TexCoord    : TEXCOORD;
};

float4 main(VertexOut input) : SV_TARGET
{
    float4 color1 = Texture[0].Sample(Sampler, input.TexCoord.xy);
    float4 color2 = Texture[1].Sample(Sampler, input.TexCoord.xy);
    float4 color = color1 * input.TexCoord.z + color2 * input.TexCoord.w;
    clip(color.a - 0.5f);
    return color;
}
