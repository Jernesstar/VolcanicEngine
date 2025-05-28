#version 450 core

uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
    FragColor = texture(u_Texture, v_TexCoords);
}