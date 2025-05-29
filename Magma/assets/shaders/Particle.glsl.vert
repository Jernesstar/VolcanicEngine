#version 450 core

struct Particle {
    vec3 Position;
    vec3 Velocity;
    float Life;
};

layout(std430, binding = 0) readonly restrict buffer SSBO_0  {
    Particle Particles[];
};

uniform mat4 u_View;
uniform mat4 u_ViewProj;
uniform float u_BillboardWidth;
uniform float u_BillboardHeight;

const vec2 Vertices[4] =
    vec2[4](
        vec2(-0.5f, -0.5f),
        vec2( 0.5f, -0.5f),
        vec2( 0.5f,  0.5f),
        vec2(-0.5f,  0.5f)
    );

const int Indices[6] = int[6](0, 2, 1, 2, 0, 3);

layout(location = 0) out vec2 v_TexCoords;

void main()
{
    Particle particle = Particles[gl_InstanceID];
    vec2 vertex = Vertices[Indices[gl_VertexID]];
    vec3 cameraRight = vec3(u_View[0][0], u_View[1][0], u_View[2][0]);
    vec3 cameraUp = vec3(u_View[0][1], u_View[1][1], u_View[2][1]);

    vec3 position =
        particle.Position
        + cameraRight * vertex.x * u_BillboardWidth
        + cameraUp * vertex.y * u_BillboardHeight;

    gl_Position = u_ViewProj * vec4(position, 1.0);

    v_TexCoords = vertex + 0.5;
}