#version 450 core

uniform mat4 u_ViewProjMatrix;

layout(location = 0) in vec4 a_VertexPosition;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TextureCoordinate;
layout(location = 3) in int a_TextureIndex;

layout(location = 1) out vec4 v_Color;
layout(location = 2) out vec2 v_TextureCoordinate;
layout(location = 3) out flat int v_TextureIndex;

void main()
{
    gl_Position = u_ViewProjMatrix * a_VertexPosition;

    v_Color = a_Color;
    v_TextureCoordinate = a_TextureCoordinate;
    v_TextureIndex = a_TextureIndex;
}