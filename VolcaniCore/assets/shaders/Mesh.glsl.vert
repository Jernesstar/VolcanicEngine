#version 450 core

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_TexCoord_Color;
// layout(location = 3) in mat4 a_InstanceMatrix;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec4 v_TexCoord_Color;
layout(location = 3) out flat int v_Textured;

void main()
{
    // v_Position = vec3(a_InstanceMatrix * vec4(a_Position, 1.0));
    v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = a_Normal;
    v_TexCoord_Color = a_TexCoord_Color;

    v_Textured = 0;
    if(v_TexCoord_Color.w == 0.0f)
        v_Textured = 1;

    gl_Position = u_ViewProj * vec4(v_Position, 1.0);
}