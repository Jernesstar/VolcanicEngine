#version 450 core

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

layout(location = 0) in vec3 a_Position;

void main()
{
    gl_Position = u_ViewProj * u_Model * vec4(a_Position, 1.0);
}