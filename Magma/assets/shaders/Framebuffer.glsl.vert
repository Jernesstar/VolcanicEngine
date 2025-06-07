#version 460 core

layout(location = 0) in vec2 a_TexCoords;

layout(location = 0) out vec2 v_TexCoords;

void main()
{
    v_TexCoords = a_TexCoords;

    gl_Position = vec4(2.0 * a_TexCoords - 1.0, 0.0, 1.0);
}