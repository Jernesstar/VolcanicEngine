#version 450 core

// uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

layout(location = 0) in vec3 a_Position;
layout(location = 3) in mat4 a_InstanceMatrix;

void main()
{
    gl_Position = u_LightSpaceMatrix * a_InstanceMatrix * vec4(a_Position, 1.0);
}