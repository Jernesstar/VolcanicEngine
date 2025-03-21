#version 450 core

uniform sampler2D u_ScreenTexture;

layout(location = 0) in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
    FragColor = vec4(texture(u_ScreenTexture, v_TexCoords).rgb, 1.0);
}
