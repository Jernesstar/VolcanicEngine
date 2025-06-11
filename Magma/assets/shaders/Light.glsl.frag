#version 460 core

layout(location = 2) uniform float u_Radius;
layout(location = 3) uniform vec3 u_CameraPosition;

layout(location = 0) in vec3 v_Center;
layout(location = 1) in vec2 v_TexCoords;

layout(location = 0) out vec4 FragColor;

vec2 SphereIntersect();

void main()
{
    // vec2 sp = SphereIntersect();
    // if(sp == vec3(-1.0))
    //     discard;

    FragColor = vec4(v_TexCoords, 1.0, 1.0);
}

vec2 SphereIntersect()
{
    float ra = u_Radius;
    vec3 ce = v_Center;
    vec3 ro = u_CameraPosition;
    vec3 rd = vec3(2.0 * gl_FragCoord.xy - 1, -1.0);

    vec3 oc = ro - ce;
    float b = dot( oc, rd );
    float c = dot( oc, oc ) - ra*ra;
    float h = b*b - c;
    if(h < 0.0)
        return vec2(-1.0); // no intersection
    h = sqrt(h);
    return vec2(-b-h, -b+h);
}
