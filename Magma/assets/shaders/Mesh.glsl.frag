#version 460 core

struct Material {
    int IsTextured;
    sampler2D Diffuse;
    vec4 DiffuseColor;
};

layout(location = 1) uniform Material u_Material;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoords;

layout(location = 0) out vec4 FragColor;

void main()
{
    vec3 color;
    if(u_Material.IsTextured == 1)
        color = texture(u_Material.Diffuse, v_TexCoords).rgb;
    else
        color = u_Material.DiffuseColor.rgb;

    FragColor = vec4(color, 1.0);
}