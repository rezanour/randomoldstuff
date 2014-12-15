#version 120

uniform mat4 World;
uniform mat4 ViewProjection;

attribute vec3 inPosition;
attribute vec3 inNormal;
attribute vec2 inUV;

varying vec3 WorldNormal;
varying vec2 UV;

void main()
{
    gl_Position = ViewProjection * World * vec4(inPosition, 1.0);
    WorldNormal = (World * vec4(inNormal, 1.0)).xyz;
    UV = inUV;
}