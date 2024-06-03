#version 450 core

uniform mat4 u_ViewProj;
uniform vec3 u_Position;

layout(location = 0) in vec3 a_Position;

void main()
{
    gl_Position = u_ViewProj * vec4(a_Position + u_Position, 1.0);
}