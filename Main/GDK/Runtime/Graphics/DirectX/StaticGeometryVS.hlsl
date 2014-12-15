#include "ShaderCommon.hlsli"

//
// This simple shader is used for non animated geometry.
//
VSOutput StaticGeometryVS(
    Vertex v    // A vertex in the goemetry
    )
{
    VSOutput output;

    // Transform and prep output
    output.ProjectedPosition = TransformPosition(v.Position);
    output.WorldNormal = TransformNormal(v.Normal);
    output.UV = v.UV;

    return output;
}
