#version 450 core

uniform mat4 u_ViewProj;
uniform mat4 u_LightSpaceMatrix;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in mat4 a_Transform;

layout(location = 0) out vec3 v_FragPos;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoords;
layout(location = 3) out vec4 v_FragPosLightSpace;

void main()
{
    v_FragPos   = vec3(a_Transform * vec4(a_Position, 1.0));
    v_Normal    = transpose(inverse(mat3(a_Transform))) * a_Normal;
    v_TexCoords = a_TexCoords;
    v_FragPosLightSpace = u_LightSpaceMatrix * vec4(v_FragPos, 1.0);

    gl_Position = u_ViewProj * vec4(v_FragPos, 1.0);
}