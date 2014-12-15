#include "ShaderCommon.hlsli"

cbuffer Globals
{
    float2 WorkAreaSize;
    float2 ViewAreaSize;
};

Output2D main( Vertex2D input )
{
    Output2D output;

    // determine position in [0, 1] space.
    float2 position = (input.Position / WorkAreaSize);

    // correct position for projection space [-1, 1]
    position.y = 1.0f - position.y;
    position = position * 2 - 1;

    output.Position = float4(position, 0.01f, 1.0f);
    output.TexCoord = input.TexCoord;

    return output;
}
