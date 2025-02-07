#version 450 core

uniform sampler2D u_ScreenTexture;

layout(location = 0) in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
    float depthValue = texture(u_ScreenTexture, v_TexCoords).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}
