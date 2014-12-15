#version 120

struct DirectionalLight
{
   vec3 Direction;
   vec4 Color;
};

uniform DirectionalLight Light1;
uniform DirectionalLight Light2;
uniform DirectionalLight Light3;

uniform sampler2D Sampler;

varying vec3 WorldNormal;
varying vec2 UV;

vec4 ComputeDirectionalLight(vec3 normal, DirectionalLight light)
{
    float NdL = dot(normal, light.Direction);
    return light.Color * clamp(NdL, 0.0, 1.0);
}

void main()
{
    vec4 samp = texture2D(Sampler, UV.xy);
    vec3 n = normalize(WorldNormal);
    gl_FragColor = samp * (ComputeDirectionalLight(n, Light1) + ComputeDirectionalLight(n, Light2) + ComputeDirectionalLight(n, Light3));
}