#version 450 core

layout(location = 0) in vec3 v_CurrentPosition;
layout(location = 1) in vec2 v_TexCoord;

uniform sampler2D u_Texture;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(texture(u_Texture, v_TexCoord)), 1.0);
}