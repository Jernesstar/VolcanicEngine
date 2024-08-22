#version 450 core

precision highp float;

uniform vec2 u_PixelSize;
uniform vec4 u_Color;
uniform sampler2D u_Texture;

in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
    const int WIDTH = 5;
    bool isInside = false;
    int count = 0;
    float coverage = 0.0;
    float dist = 1e6;

    for(int y = -WIDTH; y <= WIDTH; ++y) {
        for(int x = -WIDTH; x <= WIDTH; ++x) {
            vec2 dUV = vec2(float(x) * u_PixelSize.x, float(y) * u_PixelSize.y);
            float mask = texture2D(u_Texture, texCoord + dUV).r;
            coverage += mask;

            if(mask >= 0.5) {
                dist = min(dist, sqrt(float(x * x + y * y)));
            }
            if(x == 0 && y == 0) {
                isInside = (mask > 0.5);
            }
            count += 1;
        }
    }

    coverage /= float(count);
    float a;

    if (isInside) {
        a = min(1.0, (1.0 - coverage) / 0.75);
    } else {
        const float solid = 0.3 * float(WIDTH);
        const float fuzzy = float(WIDTH) - solid;
        a = 1.0 - min(1.0, max(0.0, dist - solid) / fuzzy);
    }

    FragColor = vec4(color, a);
}