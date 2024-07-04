#version 450 core

in vec2 v_TexCoords;

uniform sampler2D u_ScreenTexture;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(texture(u_ScreenTexture, v_TexCoords)), 1.0f);
}