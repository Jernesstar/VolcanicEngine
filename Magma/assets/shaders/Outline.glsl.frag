#version 460 core

precision highp float;

layout(location = 0) uniform sampler2D u_ScreenTexture;

layout(location = 1) uniform vec2 u_PixelSize;
layout(location = 2) uniform vec3 u_Color;
// uniform flat int u_Width;

layout(location = 0) in vec2 v_TexCoords;

layout(location = 0) out vec4 FragColor;

void main()
{
    // const int WIDTH = u_Width;
    const int WIDTH = 7;
    bool isInside = false;
    float coverage = 0.0;
    float dist = 1e6;
    int count = 0;

    for(int y = -WIDTH; y <= WIDTH; y++)
    {
        for(int x = -WIDTH; x <= WIDTH; x++)
        {
            vec2 dUV = vec2(float(x) * u_PixelSize.x, float(y) * u_PixelSize.y);
            float mask = texture(u_ScreenTexture, v_TexCoords + dUV).r;
            coverage += mask;

            if(mask >= 0.5)
                dist = min(dist, sqrt(float(x * x + y * y)));

            count += 1;
        }
    }

    // // count = pow(2 * WIDTH, 2);
    // // if(x == 0 && y == 0) {
    // //     isInside = mask > 0.5;
    // // }
    isInside = texture(u_ScreenTexture, v_TexCoords).r > 0.5;

    coverage /= float(count);
    float a;

    if (isInside)
        a = min(1.0, (1.0 - coverage) / 0.75);
    else {
        const float solid = 0.3 * float(WIDTH);
        const float fuzzy = float(WIDTH) - solid;
        a = 1.0 - min(1.0, max(0.0, dist - solid) / fuzzy);
    }

    FragColor = vec4(u_Color, a);
}