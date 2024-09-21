#version 450 core

uniform mat4 u_ViewProj;
uniform mat4 u_LightSpaceMatrix;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_TexCoords;
layout(location = 3) in mat4 a_InstanceMatrix;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec4 TexCoords;
layout(location = 3) out vec4 FragPosLightSpace;

void main()
{
    FragPos   = vec3(a_InstanceMatrix * vec4(a_Position, 1.0));
    Normal    = transpose(inverse(mat3(a_InstanceMatrix))) * a_Normal;
    TexCoords = a_TexCoords;
    FragPosLightSpace = u_LightSpaceMatrix * vec4(FragPos, 1.0);

    gl_Position = u_ViewProj * vec4(FragPos, 1.0);
}