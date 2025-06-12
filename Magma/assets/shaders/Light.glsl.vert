#version 460 core

#define MAX_POINT_LIGHTS 50

layout(location = 0) uniform mat4 u_View;
layout(location = 1) uniform mat4 u_ViewProj;
layout(location = 2) uniform float u_Radius;

struct PointLight {
    vec4 Position;
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;

    float Constant;
    float Linear;
    float Quadratic;
};

layout(std140, binding = 1) uniform PointLights
{
    PointLight Buffer[MAX_POINT_LIGHTS];
} u_PointLights;

const vec2 Vertices[4] =
    vec2[4](
        vec2(-0.5f, -0.5f),
        vec2( 0.5f, -0.5f),
        vec2( 0.5f,  0.5f),
        vec2(-0.5f,  0.5f)
    );

const int Indices[6] = int[6](0, 2, 1, 2, 0, 3);

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Center;
layout(location = 2) out vec3 v_Color;

void main()
{
    PointLight light = u_PointLights.Buffer[gl_InstanceID];
    vec3 cameraRight = vec3(u_View[0][0], u_View[1][0], u_View[2][0]);
    vec3 cameraUp = vec3(u_View[0][1], u_View[1][1], u_View[2][1]);
    vec2 vertex = Vertices[Indices[gl_VertexID]];

    v_Position =
        light.Position.xyz
        + cameraRight * vertex.x * u_Radius
        + cameraUp * vertex.y * u_Radius;

    v_Center = light.Position.xyz;
    v_Color = light.Diffuse.xyz;

    gl_Position = u_ViewProj * vec4(v_Position, 1.0);
}