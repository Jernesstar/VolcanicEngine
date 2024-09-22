#version 450 core

// uniform mat4 u_Model;
uniform mat4 u_ViewProj;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_TexCoords;
layout(location = 3) in mat4 a_InstanceMatrix;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec4 v_TexCoords;

void main()
{
    v_Position = vec3(a_InstanceMatrix * vec4(a_Position, 1.0));
    v_Normal = a_Normal;
    v_TexCoords = a_TexCoords;

    gl_Position = u_ViewProj * vec4(v_Position, 1.0);
}