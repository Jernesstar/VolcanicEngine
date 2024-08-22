#version 450 core

uniform mat4 u_ViewProj;
uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    vs_out.Normal = transpose(inverse(mat3(u_Model))) * a_Normal;
    vs_out.TexCoords = a_TexCoords;
    vs_out.FragPosLightSpace = u_LightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    gl_Position = u_ViewProj * vec4(vs_out.FragPos, 1.0);
}