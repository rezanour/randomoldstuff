
cbuffer FrameConstants
{
    float4x4 InvViewProjMatrix;
    float3 CameraPosition;
    float pad;
    float2 ScreenSize;
};

cbuffer LightConstants
{
    float3 LightDirection;
    float3 LightColor;
};

struct vsOutput
{
    float4 Position : SV_POSITION;
    float3 ViewRay : TEXCOORD;
};

Texture2D   DiffuseBuffer;
Texture2D   NormalSpecularBuffer;
Texture2D   DepthBuffer;
Texture2D   LightBuffer;

sampler PointSampler;
sampler LinearSampler;

float3 VSComputeViewRayFromProjSpacePosition( float2 projSpacePosition )
{
    float4 pos = mul(float4(projSpacePosition, 1, 1), InvViewProjMatrix);
    float3 worldPosition = pos.xyz / pos.w;
    return worldPosition - CameraPosition;
}

float2 PSGetScreenTextureCoordinates( float4 sv_position )
{
    return sv_position.xy / ScreenSize;
}

vsOutput vsFullscreenQuad(float2 position : POSITION)
{
    vsOutput output;
    output.Position = float4(position, 0, 1);
    output.ViewRay = VSComputeViewRayFromProjSpacePosition(position);
    return output;
}

float4 psDirectionalLight( vsOutput input ) : SV_TARGET0
{
    float2 texcoord = PSGetScreenTextureCoordinates(input.Position);

    float4 normalSpecular = NormalSpecularBuffer.Sample(PointSampler, texcoord);

    float3 L = -normalize(LightDirection.xyz);

    // expand normal components back out to [-1, 1]
    normalSpecular.xyz = normalSpecular.xyz * 2 - 1;

    float3 N = normalize(normalSpecular.xyz);

    float nDotL =  max(0, dot(N, L));
    float3 diffuse = LightColor.xyz * nDotL;

    return float4(diffuse, 1);
}

float4 psCombineLighting( vsOutput input ) : SV_TARGET0
{
    float2 texcoord = PSGetScreenTextureCoordinates(input.Position);
    float4 diffuse = DiffuseBuffer.Sample(PointSampler, texcoord);
    float4 lighting = LightBuffer.Sample(PointSampler, texcoord);
    return diffuse * lighting;
}
