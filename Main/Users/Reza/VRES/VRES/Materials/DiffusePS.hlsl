Texture2D Texture;
Texture2D Texture2; // Normal
sampler Sampler;

#define MAX_LIGHTS          8
#define DIRECTIONAL_LIGHT   0
#define POINT_LIGHT         1

struct Light
{
    float3 Position;    // Direction for Directional Light
    uint Type;
    float3 Color;
    float Radius;       // Only for Point
};

cbuffer Lights
{
    Light Lights[MAX_LIGHTS];
    uint NumLights;
    float3 CameraPosition;
};

struct Pixel
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float2 TexCoord : TEXCOORD;
};

float4 main(Pixel input) : SV_TARGET
{
    float4 color = Texture.Sample(Sampler, input.TexCoord);
    float3 n = normalize(input.Normal);
    float3 t = normalize(input.Tangent);
    float3 b = normalize(input.Bitangent);
    float3x3 tangentFrame = transpose(float3x3(t, b, n));

    float3 normal = (Texture2.Sample(Sampler, input.TexCoord) * 2 - 1).xyz;
    normal = mul(tangentFrame, normal);

    float3 view = normalize(CameraPosition - input.WorldPosition);

    float3 diffuseLight = 0;
    float3 specularLight = 0;

    for (uint i = 0; i < NumLights; ++i)
    {
        float attenuation = 1.0f;
        float3 lightDir = 0;

        if (Lights[i].Type == DIRECTIONAL_LIGHT)
        {
            lightDir = -Lights[i].Position;
        }
        else if (Lights[i].Type == POINT_LIGHT)
        {
            lightDir = Lights[i].Position - input.WorldPosition;
            attenuation = saturate(1.0f - (length(lightDir) / Lights[i].Radius));
            lightDir = normalize(lightDir);
        }

        float nDotL = max(0, dot(normal, lightDir));
        diffuseLight += attenuation * nDotL * Lights[i].Color;

        float3 H = normalize(lightDir + view);
        float nDotH = max(0, dot(normal, H));
        float spec = pow(nDotH, 10);
        specularLight += attenuation * spec * Lights[i].Color;
    }

    return float4(color.xyz * (diffuseLight + specularLight), color.w);
}
