#version 460 core

layout(location = 4) uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_TexCoords;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = texture(u_Texture, v_TexCoords);
}