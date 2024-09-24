#version 450 core

layout(location = 0) in vec2 TexCoords;

layout(binding = 0) uniform sampler2D u_BloomTexture;
layout(binding = 1) uniform sampler2D u_SceneTexture;

uniform float exposure;
uniform float bloomStrength = 0.04f;

vec3 bloom()
{
    vec3 bloomColor = texture(u_BloomTexture, TexCoords).rgb;
    vec3 hdrColor = texture(u_SceneTexture, TexCoords).rgb;
    return mix(hdrColor, bloomColor, bloomStrength); // linear interpolation
}

out vec4 FragColor;

void main()
{
    vec3 result = vec3(0.0);
    result = bloom();

    // tone mapping
    result = vec3(1.0) - exp(-result * exposure);

    // also gamma correct while we're at it
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}