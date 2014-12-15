struct DirectionalLight
{
    vec3 Direction;
    vec4 Color;
};

uniform DirectionalLight Light1;
uniform DirectionalLight Light2;

uniform sampler2D Sampler;

varying vec3 WorldNormal;
varying vec2 UV;
//smooth in vec3 WorldNormal;
//in vec2 UV;

//out vec4 outColor;

vec4 ComputeDirectionalLight(vec3 normal, DirectionalLight light)
{
    float NdL = dot(normal, light.Direction);
    return light.Color * clamp(NdL, 0, 1);
}

void main()
{
    vec4 samp = texture2D(Sampler, UV);
    vec3 n = normalize(WorldNormal);
    //outColor = samp * (ComputeDirectionalLight(n, Light1) + ComputeDirectionalLight(n, Light2));
    gl_FragColor = samp * (ComputeDirectionalLight(n, Light1) + ComputeDirectionalLight(n, Light2));
}
