//
// Quake 2 common shader types and methods
//

//
// Directional Light
//
struct DirectionalLight
{
    float4 Direction;       // Must be normalized!
    float4 Color;
};

//
// VS Constants
//
cbuffer GlobalVSConstants
{
    float4x4 World;             // Object-to-World transform
    float4x4 InvTransWorld;     // Transpose of the Inverse of the World transform
    float4x4 ViewProjection;    // World-to-View transform premultiplied with the View-to-Projection transform
    float AnimationLerp;        // Lerp value for blending vertex streams 1 & 2. Range is [0, 1]
};

//
// PS Constants
//
cbuffer GlobalPSConstants
{
    DirectionalLight Light1;
    DirectionalLight Light2;
};

//===============================================================================================
//===============================================================================================

//
// Helper methods
//

//
// Transform object space vertex position into projection space position suitable for rendering
//
// Assumes: World & ViewProjection constants are set
// Returns: Projected space position
float4 TransformPosition(
    in float3 position // vertex position in object-space
    )
{
    return mul(ViewProjection, mul(World, float4(position, 1)));
}

//
// Transform object space normal vector into world space normal suitable for lighting
//
// Assumes: InvTransWorld constant is set
// Returns: World space normal vector
float3 TransformNormal(
    in float3 normal // vertex normal in object space
    )
{
    return mul(InvTransWorld, float4(normal, 1)).xyz;
}

//
// Compute lighting from a directional light
//
// Assumes: normal is already normalized
// Returns: lighting value to be multiplied with surface color
float4 ComputeDirectionalLight(
    in float3 normal,           // the world space normal of the surface being lit (already normalized)
    in DirectionalLight light   // a directional light
    )
{
    float NdL = dot(normal, light.Direction.xyz);
    return light.Color * saturate(NdL);
}

//===============================================================================================
//===============================================================================================

//
// Data types
//

struct Vertex
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 ProjectedPosition : SV_POSITION;
    float3 WorldNormal : NORMAL;
    float2 UV : TEXCOORD;
};

Texture2D Texture;
sampler Sampler;
