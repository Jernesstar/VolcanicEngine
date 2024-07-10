#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in mat4 a_Model;

uniform mat4 u_LightSpaceMatrix;

void main()
{
    gl_Position = u_LightSpaceMatrix * a_Model * vec4(a_Position, 1.0);
    // gl_Position = u_LightSpaceMatrix * u_Model * vec4(a_Position, 1.0);
}