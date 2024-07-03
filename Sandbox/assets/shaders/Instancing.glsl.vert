#version 450 core

uniform mat4 u_ViewProj;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in mat4 a_InstanceMatrix;

layout(location = 0) out vec3 v_CurrentPosition;
layout(location = 1) out vec2 v_TexCoord;

void main()
{
	v_CurrentPosition = vec3(a_InstanceMatrix * vec4(a_Position, 1.0f));
	v_TexCoord = a_TexCoord;

	gl_Position = u_ViewProj * vec4(v_CurrentPosition, 1.0);
}