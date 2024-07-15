#version 450 core

in vec2 v_TexCoords;

out vec4 FragColor;

uniform sampler2D u_ScreenTexture;

void main()
{
    FragColor = vec4(vec3(texture(u_ScreenTexture, v_TexCoords)), 1.0f);
}