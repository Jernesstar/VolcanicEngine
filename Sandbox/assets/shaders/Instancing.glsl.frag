#version 450 core

layout(location = 0) in vec3 v_CurrentPosition;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}