#version 460 core

uniform vec3 u_CameraPosition;

uniform float u_GridSize = 100.0;
uniform float u_GridMinPixelsBetweenCells = 2.0;
uniform float u_GridCellSize = 0.025;
uniform vec4 u_GridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
uniform vec4 u_GridColorThick = vec4(1.0);

layout(location = 0) in vec3 v_Position;

out vec4 FragColor;

float log10(float x)
{
    float f = log(x) / log(10.0);
    return f;
}

float satf(float x)
{
    float f = clamp(x, 0.0, 1.0);
    return f;
}

vec2 satv(vec2 x)
{
    vec2 v = clamp(x, vec2(0.0), vec2(1.0));
    return v;
}

float max2(vec2 v)
{
    float f = max(v.x, v.y);
    return f;
}

void main()
{
    vec2 dvx = vec2(dFdx(v_Position.x), dFdy(v_Position.x));
    vec2 dvy = vec2(dFdx(v_Position.z), dFdy(v_Position.z));

    float lx = length(dvx);
    float ly = length(dvy);

    vec2 dudv = vec2(lx, ly);

    float l = length(dudv);

    float LOD = max(0.0, log10(l * u_GridMinPixelsBetweenCells / u_GridCellSize) + 1.0);

    float GridCellSizeLod0 = u_GridCellSize * pow(10.0, floor(LOD));
    float GridCellSizeLod1 = GridCellSizeLod0 * 10.0;
    float GridCellSizeLod2 = GridCellSizeLod1 * 10.0;

    dudv *= 4.0;

    vec2 mod_div_dudv = mod(v_Position.xz, GridCellSizeLod0) / dudv;
    float Lod0a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

    mod_div_dudv = mod(v_Position.xz, GridCellSizeLod1) / dudv;
    float Lod1a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)) );
    
    mod_div_dudv = mod(v_Position.xz, GridCellSizeLod2) / dudv;
    float Lod2a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

    float LOD_fade = fract(LOD);
    vec4 Color;

    if (Lod2a > 0.0) {
        Color = u_GridColorThick;
        Color.a *= Lod2a;
    } else {
        if (Lod1a > 0.0) {
            Color = mix(u_GridColorThick, u_GridColorThin, LOD_fade);
	        Color.a *= Lod1a;
        } else {
            Color = u_GridColorThin;
	        Color.a *= (Lod0a * (1.0 - LOD_fade));
        }
    }
    
    float OpacityFalloff = (1.0 - satf(length(v_Position.xz - u_CameraPosition.xz) / u_GridSize));

    Color.a *= OpacityFalloff;

    FragColor = Color;
}