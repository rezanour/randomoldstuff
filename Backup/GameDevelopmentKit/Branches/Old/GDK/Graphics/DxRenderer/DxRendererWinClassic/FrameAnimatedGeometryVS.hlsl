#include "ShaderCommon.hlsli"

//
// This shader is designed to lerp between two vertex buffers. You must have 2 vertex streams set
// and the strides & lengths of the streams must be equal.
// (ie. a vertex from stream 1 should line up with a vertex from stream 2)
//
VSOutput FrameAnimatedGeometryVS(
    Vertex v1,  // A vertex from the first frame in the goemetry
    Vertex v2   // A matching vertex from the second frame in the geometry
    )
{
    VSOutput output;

    // Lerp the position
    float3 positionDiff = v2.Position - v1.Position;
    float3 position = v1.Position + AnimationLerp * positionDiff;

    // Lerp the normal
    float3 normalDiff = v2.Normal - v1.Normal;
    float3 normal = v1.Normal + AnimationLerp * normalDiff;

    // Lerp the UV
    float2 uvDiff = v2.UV - v1.UV;
    float2 uv = v1.UV + AnimationLerp * uvDiff;

    // Transform and prep output
    output.ProjectedPosition = TransformPosition(position);
    output.WorldNormal = TransformNormal(normal);
    output.UV = uv;

    return output;
}
