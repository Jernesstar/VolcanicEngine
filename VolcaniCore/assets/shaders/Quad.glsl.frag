#version 450 core

layout(location = 1) in vec4 v_Color;
layout(location = 2) in vec2 v_TextureCoordinate;
layout(location = 3) in flat int v_TextureIndex;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_Textures[32];

void main()
{
    vec4 color;
    if(v_Color != vec4(0.0, 0.0, 0.0, 0.0))
    {
        if(v_TextureIndex == -1) // Flat colored quad
        {
            color = v_Color;
        }

        else // Text quad
        {
            color = v_Color * vec4(1.0, 1.0, 1.0, texture(u_Textures[v_TextureIndex], v_TextureCoordinate).r);
        }
    }

    else // Textured quad
    {
        color = texture(u_Textures[v_TextureIndex], v_TextureCoordinate);
    }

    FragColor = color;
}