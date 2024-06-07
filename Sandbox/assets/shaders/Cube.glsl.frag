#version 450 core

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoords;

uniform vec3 u_CameraPosition;
uniform sampler2D u_Texture;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(texture(u_Texture, v_TexCoords)), 1.0);
}