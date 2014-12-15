
cbuffer FrameConstants
{
    float4x4 ViewProjMatrix;
    float3 CameraPosition;
    float OneOverFarClipDistance;
};

cbuffer ObjectConstants
{
    float4x4 WorldMatrix;
};

struct vsInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct vsOutput
{
    float4 Position : SV_POSITION;
    float3 WorldNormal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float3 ViewVector : TEXCOORD1;
};

vsOutput vsSimpleShader (vsInput input)
{
    vsOutput output;

    output.Position = mul(float4(input.Position, 1), mul(WorldMatrix, ViewProjMatrix));
    output.WorldNormal = mul(float4(input.Normal, 0), WorldMatrix).xyz;
    output.TexCoord = input.TexCoord;
    output.ViewVector = CameraPosition - mul(float4(input.Position, 1), WorldMatrix).xyz;

    return output;
}

struct psOutput
{
    float4 Diffuse : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 ViewDepth : SV_TARGET2;
};

Texture2D DiffuseTexture;
sampler LinearSampler;

psOutput psSimpleShader (vsOutput input)
{
    psOutput output;

    output.Diffuse.xyz = DiffuseTexture.Sample(LinearSampler, input.TexCoord).xyz;
    output.Diffuse.w = 0; // specular

    output.Normal.xyz = (normalize(input.WorldNormal) + 1) * 0.5f;
    output.Normal.w = 0; // specular

    output.ViewDepth.x = length(input.ViewVector) * OneOverFarClipDistance;

    return output;
}
