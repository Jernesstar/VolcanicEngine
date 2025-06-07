#version 460 core

// This shader performs upsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.

// Remember to add bilinear minification filter for this texture!
// Remember to use a floating-point texture format (for HDR)!
// Remember to use edge clamping for this texture!

layout(location = 1) uniform float u_FilterRadius;
layout(location = 2) uniform vec2 u_SrcResolution;
layout(location = 3, binding = 0) uniform sampler2D u_SrcTexture;

layout(location = 0) in vec2 v_TexCoords;

layout(location = 0) out vec4 FragColor;

void main()
{
    // The filter kernel is applied with a radius, specified in texture
    // coordinates, so that the radius will vary across mip resolutions.
    float aspectRatio = u_SrcResolution.x / u_SrcResolution.y;
    float x = u_FilterRadius;
    float y = u_FilterRadius * aspectRatio;

    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // === ('e' is the current texel) ===
    vec3 a = texture(u_SrcTexture, vec2(v_TexCoords.x - x, v_TexCoords.y + y)).rgb;
    vec3 b = texture(u_SrcTexture, vec2(v_TexCoords.x,     v_TexCoords.y + y)).rgb;
    vec3 c = texture(u_SrcTexture, vec2(v_TexCoords.x + x, v_TexCoords.y + y)).rgb;

    vec3 d = texture(u_SrcTexture, vec2(v_TexCoords.x - x, v_TexCoords.y)).rgb;
    vec3 e = texture(u_SrcTexture, vec2(v_TexCoords.x,     v_TexCoords.y)).rgb;
    vec3 f = texture(u_SrcTexture, vec2(v_TexCoords.x + x, v_TexCoords.y)).rgb;

    vec3 g = texture(u_SrcTexture, vec2(v_TexCoords.x - x, v_TexCoords.y - y)).rgb;
    vec3 h = texture(u_SrcTexture, vec2(v_TexCoords.x,     v_TexCoords.y - y)).rgb;
    vec3 i = texture(u_SrcTexture, vec2(v_TexCoords.x + x, v_TexCoords.y - y)).rgb;

    // Apply weighted distribution, by using a 3x3 tent filter:
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |

    vec3 upsample;
    upsample = e * 4.0;
    upsample += (b + d + f + h) * 2.0;
    upsample += (a + c + g + i);
    upsample *= 1.0 / 16.0;

    FragColor = vec4(upsample, 1.0);
}