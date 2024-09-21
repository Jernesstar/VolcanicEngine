#version 450 core

uniform sampler2D u_Diffuse;
uniform sampler2D u_Specular;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec4 v_TexCoord_Color;
layout(location = 3) in flat int v_Textured;

out vec4 FragColor;

void main()
{
    vec3 color = v_TexCoord_Color.xyz;
    if(v_Textured == 1)
        color = texture(u_Diffuse, v_TexCoord_Color.xy).rgb;

    FragColor = vec4(color, 1.0);
}