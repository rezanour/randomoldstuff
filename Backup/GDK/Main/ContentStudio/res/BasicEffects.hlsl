// General vertex shader inputs (not all apply to each shader)

float4x4    World       : WORLD;
float4x4    InvWorld    : INVWORLD;
float4x4    View        : VIEW;
float4x4    Projection  : PROJECTION;

// Vertex Input Formats

struct VSI_DiffuseOnly
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct VSI_DiffuseNormalMap
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texcoord : TEXCOORD;
};

struct VSI_ProjSpaceQuad
{
    float2 position : POSITION;
    float2 texcoord : TEXCOORD;
};

// Vertex outputs (Pixel/Geometry shader inputs)

struct VSO_DiffuseOnly
{
    float4 position : SV_POSITION;
    float3 worldNormal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct VSO_DiffuseNormalMap
{
    float4 position : SV_POSITION;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float3 worldBiTangent : BITANGENT;
    float2 texcoord : TEXCOORD;
};

struct VSO_ProjSpaceQuad
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

// Vertex Shaders

VSO_DiffuseOnly vsDiffuseOnly ( VSI_DiffuseOnly input )
{
    VSO_DiffuseOnly output;
    output.position = mul(mul(mul(float4(input.position, 1), World), View), Projection);
    output.worldNormal = mul(float4(input.normal, 0), transpose(InvWorld)).xyz;
    output.texcoord = input.texcoord;
    return output;
}

VSO_DiffuseNormalMap vsDiffuseNormalMap ( VSI_DiffuseNormalMap input )
{
    VSO_DiffuseNormalMap output;
    output.position = mul(mul(mul(float4(input.position, 1), World), View), Projection);
    float4x4 transInvWorld = transpose(InvWorld);
    output.worldNormal = mul(float4(input.normal, 0), transInvWorld).xyz;
    output.worldTangent = mul(float4(input.tangent, 0), transInvWorld).xyz;
    output.worldBiTangent = mul(float4(input.bitangent, 0), transInvWorld).xyz;
    output.texcoord = input.texcoord;
    return output;
}

VSO_ProjSpaceQuad vsProjSpaceQuad ( VSI_ProjSpaceQuad input )
{
    VSO_ProjSpaceQuad output;
    output.position = float4(input.position.xy, 0, 1);
    output.texcoord = input.texcoord;
    return output;
}

// Geometry Shaders

Texture2D gsTexture;
//float AspectRatio;

[maxvertexcount(3)]
void gsFitTextureQuad ( triangle VSO_ProjSpaceQuad vertices[3], inout TriangleStream<VSO_ProjSpaceQuad> outputStream )
{
    float AspectRatio = 16.0 / 9.0;

    uint width, height;
    gsTexture.GetDimensions(width, height);

    float scale = 0.95 / max(width, height);
    float xScale = scale * (float)width / AspectRatio;
    float yScale = scale * (float)height;

    for (uint i = 0; i < 3; i++)
    {
        VSO_ProjSpaceQuad output;

        output.position = vertices[i].position;
        output.position.x *= xScale;
        output.position.y *= yScale;
        output.texcoord = vertices[i].texcoord;

        outputStream.Append(output);
    }

    outputStream.RestartStrip();
}


// Pixel Shaders

float3 ComputeDiffuse ( float3 N, float3 L, float3 LightColor )
{
    float nDotL = saturate(dot(N, L));
    return LightColor * nDotL;
}

Texture2D DiffuseTexture;
Texture2D NormalMapTexture;

sampler DiffuseSampler;
sampler NormalMapSampler;

struct DirectionalLight
{
    float4 direction;
    float4 color;
};

DirectionalLight DLight1;
DirectionalLight DLight2;
DirectionalLight DLight3;

float4 psDiffuseOnly ( VSO_DiffuseOnly input ) : SV_TARGET0
{
    float4 materialColor = DiffuseTexture.Sample(DiffuseSampler, input.texcoord);

    float3 normal = normalize ( input.worldNormal );

    float3 totalDiffuse = float3(0, 0, 0);
    totalDiffuse += ComputeDiffuse( normal, DLight1.direction.xyz, DLight1.color.xyz );
    totalDiffuse += ComputeDiffuse( normal, DLight2.direction.xyz, DLight2.color.xyz );
    totalDiffuse += ComputeDiffuse( normal, DLight3.direction.xyz, DLight3.color.xyz );

    return materialColor * float4(totalDiffuse, 1);
}

float4 psDiffuseNormalMap ( VSO_DiffuseNormalMap input ) : SV_TARGET0
{
    float4 materialColor = DiffuseTexture.Sample(DiffuseSampler, input.texcoord);
    float3 normal = NormalMapTexture.Sample(NormalMapSampler, input.texcoord).xyz * 2 - 1;

    float3x3 tangToWorld = transpose( float3x3(normalize(input.worldTangent), normalize(input.worldBiTangent), normalize(input.worldNormal)) );
    normal = mul(normal, tangToWorld);

    float3 totalDiffuse = float3(0, 0, 0);
    totalDiffuse += ComputeDiffuse( normal, DLight1.direction.xyz, DLight1.color.xyz );
    totalDiffuse += ComputeDiffuse( normal, DLight2.direction.xyz, DLight2.color.xyz );
    totalDiffuse += ComputeDiffuse( normal, DLight3.direction.xyz, DLight3.color.xyz );

    return materialColor * float4(totalDiffuse, 1);
}

float4 psFitTextureQuad ( VSO_ProjSpaceQuad input ) : SV_TARGET0
{
    return DiffuseTexture.Sample(DiffuseSampler, input.texcoord);
}


