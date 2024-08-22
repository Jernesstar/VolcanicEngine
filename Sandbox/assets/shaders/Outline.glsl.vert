#version 450 core

uniform vec2 u_PixelSize;

layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

void main()
{
    v_TexCoords = a_TexCoords;

    gl_Position = vec4(a_Pos.x, a_Pos.y, 0.0, 1.0);
}