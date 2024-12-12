#version 450 core

uniform sampler2D u_Diffuse;
uniform sampler2D u_Specular;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;

out vec4 FragColor;

void main()
{
    vec3 color;
    color = texture(u_Diffuse, v_TexCoord.xy).rgb;

    FragColor = vec4(color, 1.0);
}