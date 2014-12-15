//==============================================================================
//  PixelShader for drawing 2D images in screen space
//==============================================================================

Texture2D Texture;
sampler Sampler;

struct psDraw2DInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 main(psDraw2DInput input) : SV_TARGET
{
    float4 color = Texture.Sample(Sampler, input.TexCoord);
    clip(color.w - 0.05f);
    return color;
}
