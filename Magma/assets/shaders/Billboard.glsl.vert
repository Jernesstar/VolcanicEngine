#version 450 core

uniform mat4 u_View;
uniform mat4 u_ViewProj;
uniform float u_BillboardWidth;
uniform float u_BillboardHeight;

const vec2 Vertices[4] =
    vec2[4](
        vec2(-0.5f, -0.5f),
        vec2( 0.5f, -0.5f),
        vec2(-0.5f,  0.5f),
        vec2( 0.5f,  0.5f)
    );

const int Indices[6] = int[6](0, 2, 1, 2, 0, 3);

layout(location = 0) in vec3 a_BillboardCenter;

layout(location = 0) out vec3 v_Position;

void main()
{
    vec3 cameraRight = vec3(u_View[0][0], u_View[1][0], u_View[2][0]);
    vec3 cameraUp = vec3(u_View[0][1], u_View[1][1], u_View[2][1]);
    vec2 vertex = Vertices[Indices[gl_VertexID]];

    v_Position =
        a_BillboardCenter
        + cameraRight * vertex.x * u_BillboardWidth
        + cameraUp * vertex.y * u_BillboardHeight;

    gl_Position = u_ViewProj * vec4(v_Position, 1.0);
}