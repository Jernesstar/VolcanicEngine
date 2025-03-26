#version 450 core

uniform vec3 u_CameraPosition;
uniform mat4 u_ViewProj = mat4(1.0);
uniform float u_GridSize = 100.0;

const vec3 Vertices[4] =
    vec3[4](
        vec3(-1.0, 0.0, -1.0),
        vec3( 1.0, 0.0, -1.0),
        vec3( 1.0, 0.0,  1.0),
        vec3(-1.0, 0.0,  1.0)
    );

const int Indices[6] = int[6](0, 2, 1, 2, 0, 3);

layout(location = 0) out vec3 v_Position;

void main()
{
    int idx = Indices[gl_VertexID];
    vec3 pos = Vertices[idx] * u_GridSize;
    pos.x += u_CameraPosition.x;
    pos.z += u_CameraPosition.z;

    gl_Position = u_ViewProj * vec4(pos, 1.0);
    v_Position = pos;
}