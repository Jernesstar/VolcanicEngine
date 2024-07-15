#version 450 core

layout(location = 0) in vec2 v_TexCoords;

layout(location = 0) out vec4 FragColor;

uniform sampler2D u_ScreenTexture;

void main()
{
    FragColor = vec4(vec3(texture(u_ScreenTexture, v_TexCoords)), 1.0f);
}