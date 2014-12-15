uniform mat4 World;
uniform mat4 InvTransWorld;
uniform mat4 ViewProjection;

attribute vec3 inPosition;
attribute vec3 inNormal;
attribute vec2 inUV;
//layout (location = 0) in vec3 inPosition;
//layout (location = 1) in vec3 inNormal;
//layout (location = 2) in vec2 inUV;

varying vec3 WorldNormal;
varying vec2 UV;
//smooth out vec3 WorldNormal;
//out vec2 UV;

void main()
{
    gl_Position = ViewProjection * World * vec4(inPosition, 1.0);
    WorldNormal = (InvTransWorld * vec4(inNormal, 1.0)).xyz;
    UV = inUV;
}
