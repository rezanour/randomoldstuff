float4x4    g_World;
float4x4    g_InvWorld;
float4x4    g_View;
float4x4    g_Projection;
float3      g_EyePosition;

struct VS_INPUT
{
    float3  Position    :   POSITION;
    float3  Normal      :   NORMAL;
    float2  TexCoord    :   TEXCOORD;
};

struct VS_INPUT2
{
    float3  Position    :   POSITION;
    float3  Normal      :   NORMAL;
    float3  Tangent     :   TANGENT;
    float3  BiTangent   :   BITANGENT;
    float2  TexCoord    :   TEXCOORD;
};

struct VS_OUTPUT
{
    float4  Position        :   SV_POSITION;
    float3  Normal          :   TEXCOORD0;
    float3  View            :   TEXCOORD1;
    float2  TexCoord        :   TEXCOORD2;
};

struct VS_OUTPUT2
{
    float4  Position        :   SV_POSITION;
    float3  Normal          :   TEXCOORD0;
    float3  View            :   TEXCOORD1;
    float2  TexCoord        :   TEXCOORD2;
    float3x3 TangToWorld    :   TEXCOORD3;
};

VS_OUTPUT vsBasicEffect ( VS_INPUT input )
{
    VS_OUTPUT output;

    float3 worldPosition = mul( float4( input.Position, 1 ), g_World ).xyz;
    output.Position = mul( mul( mul( float4(input.Position, 1), g_World ), g_View), g_Projection );
    output.Normal = mul( float4( input.Normal, 1 ), transpose(g_InvWorld) );
    output.View = g_EyePosition - worldPosition;
    output.TexCoord = input.TexCoord;

    return output;
}

VS_OUTPUT2 vsBasicEffect2 ( VS_INPUT2 input )
{
    VS_OUTPUT2 output;

    float3 worldPosition = mul( float4( input.Position, 1 ), g_World ).xyz;
    output.Position = mul( mul( mul( float4(input.Position, 1), g_World ), g_View), g_Projection );
    output.Normal = mul( float4( input.Normal, 1 ), transpose(g_InvWorld) );
    output.View = g_EyePosition - worldPosition;
    output.TexCoord = input.TexCoord;

    float4x4 tangToWorld = mul( float4x4( float4( input.Tangent, 0 ), float4( -input.BiTangent, 0 ), float4( input.Normal, 0 ), float4( 0, 0, 0, 1 ) ), g_World );
    output.TangToWorld = float3x3( tangToWorld[0].xyz, tangToWorld[1].xyz, tangToWorld[2].xyz );

    return output;
}

