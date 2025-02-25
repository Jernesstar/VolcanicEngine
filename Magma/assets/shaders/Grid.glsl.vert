#version 450 core

out vec3 WorldPos;

uniform vec3 u_CameraPosition;
uniform mat4 gVP = mat4(1.0);
uniform float gGridSize = 100.0;

const vec3 Pos[4] = vec3[4](
    vec3(-1.0, 0.0, -1.0),
    vec3( 1.0, 0.0, -1.0),
    vec3( 1.0, 0.0,  1.0),
    vec3(-1.0, 0.0,  1.0)
);

const int Indices[6] = int[6](0, 2, 1, 2, 0, 3);

void main()
{
    int Index = Indices[gl_VertexID];
    vec3 vPos3 = Pos[Index] * gGridSize;

    vPos3.x += u_CameraPosition.x;
    vPos3.z += u_CameraPosition.z;

    vec4 vPos4 = vec4(vPos3, 1.0);

    gl_Position = gVP * vPos4;

    WorldPos = vPos3;
}