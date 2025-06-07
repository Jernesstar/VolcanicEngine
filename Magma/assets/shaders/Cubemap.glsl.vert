#version 460 core

uniform mat4 u_View;
uniform mat4 u_Proj;

layout (location = 0) in vec3 a_VertexPosition;

layout(location = 0) out vec3 v_TextureCoordinate;

void main()
{
    vec4 pos = u_Proj * u_View * vec4(a_VertexPosition, 1.0);
    gl_Position = pos.xyww;
    v_TextureCoordinate = a_VertexPosition;
}