#version 450 core

#define POINT_LIGHTS 4

layout(location = 0) in vec3 v_FragPosition;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TextureCoordinate;

layout(location = 0) out vec4 FragColor;

struct Material {
    sampler2D Diffuse;
    sampler2D Specular;
    float Shininess;
};

// struct DirectionalLight {
//     vec3 Position;
//     vec3 Direction;

//     vec3 Ambient;
//     vec3 Diffuse;
//     vec3 Specular;
// };

struct PointLight {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Constant;
    float Linear;
    float Quadratic;
};

layout(binding = 0, std140) uniform SpotLight
{
    vec4 sPosition;
    vec4 sDirection;

    vec4 sAmbient;
    vec4 sDiffuse;
    vec4 sSpecular;

    float sCutoffAngle;
    float sOuterCutoffAngle;
};

uniform vec3 u_CameraPosition;
uniform Material u_Material;
uniform PointLight u_PointLights[POINT_LIGHTS];

// vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 view_dir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir);
vec3 CalcSpotLight(vec3 normal, vec3 view_dir);

void main()
{
    vec3 normal = normalize(v_Normal);
    vec3 view_dir = normalize(u_CameraPosition - v_FragPosition);

    vec3 result = vec3(0.0, 0.0, 0.0);
    result += CalcSpotLight(normal, view_dir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcSpotLight(vec3 normal, vec3 view_dir)
{
    float cutoff = cos(sCutoffAngle);
    float outer = cos(sOuterCutoffAngle);

    vec3 light_dir = normalize(v_FragPosition - sPosition.xyz);
    vec3 reflect_dir = reflect(light_dir, normal);
    float diff = clamp(dot(normal, -light_dir), 0.0, 1.0);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_Material.Shininess);

    float theta = dot(-light_dir, -normalize(sDirection.xyz));
    float epsilon = cutoff - outer;
    float intensity = clamp((theta - outer) / epsilon, 0.0, 1.0);

    vec3 sAmbient = vec3(1.0, 1.0, 1.0);
    vec3 sDiffuse = vec3(1.0, 1.0, 1.0);
    vec3 sSpecular = vec3(1.0, 1.0, 1.0);

    vec3 ambient  = sAmbient  * 1.0  * vec3(texture(u_Material.Diffuse, v_TextureCoordinate));
    vec3 diffuse  = sDiffuse  * diff * vec3(texture(u_Material.Diffuse, v_TextureCoordinate));
    vec3 specular = sSpecular * spec * vec3(texture(u_Material.Specular, v_TextureCoordinate));

    return (diffuse + specular) * intensity;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir)
{
    vec3 light_dir = normalize(v_FragPosition - light.Position);
    vec3 reflect_dir = reflect(light_dir, normal);
    float diff = max(dot(normal, -light_dir), 0.0);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_Material.Shininess);

    float dist = length(light.Position - v_FragPosition);
    float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));

    vec3 ambient  = light.Ambient  * 1.0  * vec3(texture(u_Material.Diffuse, v_TextureCoordinate));
    vec3 diffuse  = light.Diffuse  * diff * vec3(texture(u_Material.Diffuse, v_TextureCoordinate));
    vec3 specular = light.Specular * spec * vec3(texture(u_Material.Specular, v_TextureCoordinate));

    return (ambient + diffuse + specular) * attenuation;
}

// vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 view_dir)
// {
//     vec3 light_dir = normalize(light.Direction);
//     vec3 reflect_dir = reflect(light_dir, normal);
//     float diff = max(dot(normal, -light_dir), 0.0);
//     float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_Material.Shininess);

//     vec3 ambient  = light.Ambient  * 1.0  * vec3(texture(u_Material.Diffuse, v_TextureCoordinate));
//     vec3 diffuse  = light.Diffuse  * diff * vec3(texture(u_Material.Diffuse, v_TextureCoordinate));
//     vec3 specular = light.Specular * spec * vec3(texture(u_Material.Specular, v_TextureCoordinate));
//     return (ambient + diffuse + specular);
// }