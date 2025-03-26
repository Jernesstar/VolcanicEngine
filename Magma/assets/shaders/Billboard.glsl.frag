#version 450 core

uniform int u_LightType;

layout(location = 0) in vec3 v_Position;

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0);
}