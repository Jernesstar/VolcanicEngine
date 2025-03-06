#version 450 core

struct Material {
    sampler2D Diffuse;
    sampler2D Specular;
    vec4 DiffuseColor;
    vec4 SpecularColor;

    int IsTextured;
};

uniform Material u_Material;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
    vec3 color;
    if(u_Material.IsTextured == 1)
        color = texture(u_Material.Diffuse, v_TexCoords).rgb;
    else
        color = u_Material.DiffuseColor.rgb;

    // FragColor = vec4(color, 1.0);
    FragColor = vec4(1.0);
}