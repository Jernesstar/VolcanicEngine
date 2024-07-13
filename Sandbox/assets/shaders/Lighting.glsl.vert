#version 450 core

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;

void main()
{
    gl_Position = u_ViewProj * u_Model * vec4(a_Position, 1.0);

    v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = a_Normal;
    v_TextureCoordinate = a_TextureCoordinate;
}