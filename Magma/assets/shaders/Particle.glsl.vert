#version 450 core

struct Particle {
    vec3 Position;
    vec3 Velocity;
    float Life;
};

uniform mat4 u_View;
uniform mat4 u_ViewProj;
uniform float u_BillboardWidth;
uniform float u_BillboardHeight;

layout(std430, binding = 0) readonly restrict buffer SSBO_1  {
    Particle Particles[];
};

const vec2 Vertices[4] =
    vec2[4](
        vec2(-0.5f, -0.5f),
        vec2( 0.5f, -0.5f),
        vec2( 0.5f,  0.5f),
        vec2(-0.5f,  0.5f)
    );

const int Indices[6] = int[6](0, 2, 1, 2, 0, 3);

layout(location = 0) in vec3 a_BillboardCenter;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec2 v_TexCoords;

void main()
{
    vec3 cameraRight = vec3(u_View[0][0], u_View[1][0], u_View[2][0]);
    vec3 cameraUp = vec3(u_View[0][1], u_View[1][1], u_View[2][1]);
    vec2 vertex = Vertices[Indices[gl_VertexID]];

    Particle particle = Particles[gl_InstanceID];
    if(particle.Life <= 0)
        return;

    v_Position =
        a_BillboardCenter
        + cameraRight * vertex.x * u_BillboardWidth
        + cameraUp * vertex.y * u_BillboardHeight;
    v_TexCoords = vertex + 0.5;

    gl_Position = u_ViewProj * vec4(v_Position, 1.0);
}