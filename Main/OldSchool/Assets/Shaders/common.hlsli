//==============================================================================
//  Common definitions, types, and helper methods
//==============================================================================

#define MAX_SPECULAR 300 // Shiniest value. Specular maps are 0 -> 1 * this value

// Gets the ray from the eye passing through the position in view space
float3 GetViewRay(float3 viewPosition)
{
    return float3(viewPosition.xy / viewPosition.z, 1.0f);
}

// Gets pixel coordinates for sampling full screen surfaces using mip 0
int3 GetPixelCoord(float4 svPosition)
{
    return int3(svPosition.xy, 0);
}

// Packs a vector from [-1, 1] to [0, 1] range
float3 PackVector(float3 v)
{
    return v * 0.5 + 0.5;
}

// Unpacks a vector that's been packed into the [0, 1] range out to the full [-1, 1] range
float3 UnpackVector(float3 v)
{
    return v * 2 - 1;
}

// Compute linear depth value from logDepth (what's in the zbuffer) and using the m33 and m44 elements of the projection matrix
float GetLinearDepth(float logDepth, float proj33, float proj43)
{
    return proj43 / (logDepth - proj33);
}

float3 ReconstructViewPosition(float3 viewRay, float logDepth, float proj33, float proj43)
{
    return viewRay * GetLinearDepth(logDepth, proj33, proj43);
}

float3 ComputeDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    float nDotL = max(0, dot(normal, lightDirection));
    return lightColor * nDotL;
}

float3 ComputeSpecular(float3 lightDirection, float3 lightColor, float3 normal, float3 view, float shininess)
{
    float3 H = normalize(view + lightDirection);
    float spec = max(0, pow(dot(normal, H), shininess));
    return lightColor * spec;
}

uint PackColor(float4 color)
{
    return ((uint)(color.x * 255) << 24) |
        ((uint)(color.y * 255) << 16) |
        ((uint)(color.z * 255) << 8) |
        ((uint)(color.w * 255));
}

float4 UnpackColor(uint color)
{
    return float4((color >> 24) / 255.0f, ((color & 0x00FF0000) >> 16) / 255.0f, ((color & 0x0000FF00) >> 8) / 255.0f, (color & 0x000000FF) / 255.0f);
}

//
// Used for GBuffer & Transparent rendering
//

struct psPositionNormalTangentInput
{
    float4 Position : SV_POSITION;
    float3 ViewPosition : POSITION;
    float3 ViewNormal : NORMAL;
    float3 ViewTangent : TANGENT;
    float3 ViewBitangent : BITANGENT;
    float2 TexCoord : TEXCOORD;
};

//
// Used for Order Independent Transparency (OIT) rendering
//

struct OITNode
{
    uint Color;
    float Depth;
    uint Next;
};
