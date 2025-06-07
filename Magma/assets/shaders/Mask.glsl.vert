#version 460 core

uniform mat4 u_ViewProj;

layout(location = 0) in vec3 a_Position;
layout(location = 3) in mat4 a_Transform;

void main()
{
    gl_Position = u_ViewProj * a_Transform * vec4(a_Position, 1.0);
}