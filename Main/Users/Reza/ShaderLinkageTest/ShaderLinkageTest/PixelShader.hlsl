sampler Sampler;

struct INPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
};

float4 main(INPUT input) : SV_TARGET
{
    // blue dir light
    float3 lightDir = normalize(float3(1.0f, 3.0f, -2.0f));
    float NdL = saturate(dot(normalize(input.Normal), lightDir));
    float3 directionalLight = float3(0, 0.4, 0.8) * NdL;

    // red point light
    float3 lightPos = float3(-8, 5, -5);
    float lightRadius = 10;
    float3 L = lightPos - input.WorldPosition;
    float3 attenuation = saturate(1 - dot(L/lightRadius, L/lightRadius));
    float3 pointLight = float3(1, 0, 0) * attenuation;

    // green point light
    lightPos = float3(8, 0, -1);
    lightRadius = 20;
    L = lightPos - input.WorldPosition;
    attenuation = saturate(1 - dot(L/lightRadius, L/lightRadius));
    float3 pointLight2 = float3(0, 0.5, 0) * attenuation;

    return float4(directionalLight + pointLight + pointLight2, 1.0f);
}
