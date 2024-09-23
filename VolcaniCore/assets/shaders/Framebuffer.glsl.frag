#version 450 core

layout(binding = 0) uniform sampler2D u_ScreenTexture;

layout(location = 0) in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(texture(u_ScreenTexture, v_TexCoords)), 1.0f);
}