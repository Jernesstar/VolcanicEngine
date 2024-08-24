#version 450 core

precision highp float;

uniform vec2 u_PixelSize;
uniform vec4 u_Color;
// uniform flat int u_Width;
uniform sampler2D u_Texture;

in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
    const int WIDTH = 5;
    bool isInside = false;
    float coverage = 0.0;
    float dist = 1e6;
    int count = 0;

    bool up    = texture(u_Texture, vec2(v_TexCoords.x, v_TexCoords.y + WIDTH * u_PixelSize.y)).r > 0.5;
    bool down  = texture(u_Texture, vec2(v_TexCoords.x, v_TexCoords.y - WIDTH * u_PixelSize.y)).r > 0.5;
    bool left  = texture(u_Texture, vec2(v_TexCoords.x - WIDTH * u_PixelSize.x, v_TexCoords.y)).r > 0.5;
    bool right = texture(u_Texture, vec2(v_TexCoords.x + WIDTH * u_PixelSize.x, v_TexCoords.y)).r > 0.5;

    if(!up && !down && !left && !right) {
        FragColor = vec4(0.0);
        return;
    }

    for(int y = -WIDTH; y <= WIDTH; y++)
    {
        for(int x = -WIDTH; x <= WIDTH; x++)
        {
            vec2 dUV = vec2(float(x) * u_PixelSize.x, float(y) * u_PixelSize.y);
            float mask = texture(u_Texture, v_TexCoords + dUV).r;
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
    isInside = texture(u_Texture, v_TexCoords).r > 0.5;

    coverage /= float(count);
    float a;

    if (isInside)
        a = min(1.0, (1.0 - coverage) / 0.75);
    else {
        const float solid = 0.3 * float(WIDTH);
        const float fuzzy = float(WIDTH) - solid;
        a = 1.0 - min(1.0, max(0.0, dist - solid) / fuzzy);
    }

    FragColor = vec4(u_Color.xyz, a);
}