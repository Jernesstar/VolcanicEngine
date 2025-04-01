#version 450 core

uniform float u_Exposure;
uniform float u_BloomStrength;

layout(binding = 0) uniform sampler2D u_BloomTexture;
layout(binding = 1) uniform sampler2D u_SceneTexture;

layout(location = 0) in vec2 v_TexCoords;

vec3 bloom()
{
    vec3 bloomColor = texture(u_BloomTexture, v_TexCoords).rgb;
    vec3 hdrColor = texture(u_SceneTexture, v_TexCoords).rgb;
    return mix(hdrColor, bloomColor, u_BloomStrength); // linear interpolation
}

out vec4 FragColor;

void main()
{
    vec3 result = vec3(0.0);
    result = bloom();

    // tone mapping
    result = vec3(1.0) - exp(-result * u_Exposure);

    // also gamma correct while we're at it
    float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}