#version 450 core

uniform sampler2D u_Texture;

layout(location = 0) in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
    // FragColor = vec4(texture(u_Texture, v_TexCoords).rgb, 1.0);
    FragColor = vec4(v_TexCoords, 1.0, 1.0);
}