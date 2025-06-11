#version 460 core

layout(location = 2) uniform float u_Radius;
layout(location = 3) uniform vec3 u_CameraPosition;
layout(location = 4) uniform float u_ViewportWidth;
layout(location = 5) uniform float u_ViewportHeight;

layout(location = 0) in vec3 v_Center;
layout(location = 1) in vec3 v_Color;

layout(location = 0) out vec4 FragColor;

vec2 SphereIntersect();

void main()
{
    // vec2 sp = SphereIntersect();
    // if(sp == vec2(-1.0))
    //     FragColor = vec4(0.0);
    // else
    //     FragColor = vec4(sp, 0.0, 1.0);

    FragColor = vec4(v_Color, 1.0);
}

vec2 SphereIntersect()
{
    float ra = u_Radius;
    vec3 ce = v_Center;
    vec3 ro = u_CameraPosition;
    vec2 uv = gl_FragCoord.xy / vec2(u_ViewportWidth, u_ViewportHeight);
    // uv *= u_ViewportWidth / u_ViewportHeight;
    vec3 rd = vec3(uv, -1.0);

    vec3 oc = ro - ce;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - ra*ra;
    float h = b*b - c;
    if(h < 0.0) // no intersection
        return vec2(-1.0);

    h = sqrt(h);
    return vec2(-b-h, -b+h);
}
