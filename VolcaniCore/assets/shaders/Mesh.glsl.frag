#version 450 core

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;

uniform sampler2D u_Diffuse;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(texture(u_Diffuse, v_TexCoord)), 1.0);
    // FragColor = vec4(1.0);
}