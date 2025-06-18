#version 460 core

layout(location = 1) uniform vec4 u_Color;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = u_Color;
}