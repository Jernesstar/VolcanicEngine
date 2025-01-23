#version 450 core

uniform sampler2D u_ScreenTexture;

layout(location = 0) in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
    // FragColor = vec4(0.01, 0.6, 1.0, 1.0);
    FragColor = vec4(texture(u_ScreenTexture, v_TexCoords).r, 0.0, 0.0, 1.0);
}
