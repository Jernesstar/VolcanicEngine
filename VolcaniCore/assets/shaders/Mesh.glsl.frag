#version 450 core

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec4 v_TexCoord_Color;
layout(location = 3) in flat int v_Textured;

uniform sampler2D u_Diffuse;

layout(location = 0) out vec4 FragColor;

void main()
{
    // if(v_Textured == 1)
    //     FragColor = vec4(vec3(texture(u_Diffuse, v_TexCoord_Color.xy)), 1.0);
    // else
    //     FragColor = v_TexCoord_Color;
    // FragColor = vec4(FragColor, 1.0);
    FragColor = vec4(v_Position, 1.0);
}