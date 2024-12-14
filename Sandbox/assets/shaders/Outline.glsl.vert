#version 450 core

layout(location = 0) in vec2 a_TexCoords;

out vec2 v_TexCoords;

void main()
{
    gl_Position = vec4(2.0*a_TexCoords - 1.0, 0.0, 1.0);
    v_TexCoords = a_TexCoords;
}