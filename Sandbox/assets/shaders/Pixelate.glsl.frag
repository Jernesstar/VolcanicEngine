#version 450 core

in vec2 v_TexCoords;

uniform sampler2D u_ScreenTexture;

out vec4 FragColor;

void main()
{
    float pixels = 1024.0;
    float dx = 5.0 * (1.0 / pixels);
    float dy = 5.0 * (1.0 / pixels);
    vec2 coord = vec2(dx * floor(v_TexCoords.x / dx), dy * floor(v_TexCoords.y / dy));
    FragColor = vec4(vec3(texture(u_ScreenTexture, coord)), 1.0);
    // FragColor = vec4(vec3(texture(u_ScreenTexture, v_TexCoords)), 1.0);
    // FragColor = vec4(coord.x, coord.y, 1.0, 1.0);
}