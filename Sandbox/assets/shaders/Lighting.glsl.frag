#version 450 core

#define MAX_POINT_LIGHTS 16

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;

layout(location = 0) out vec4 FragColor;

struct Material {
    sampler2D Diffuse;
    sampler2D Specular;
    float Shininess;
};

struct PointLight {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Constant;
    float Linear;
    float Quadratic;
};

struct DirectionalLight {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    vec3 Direction;
};

struct SpotLight {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float CutoffAngle;
    float OuterCutoffAngle;
}

uniform vec3 u_CameraPosition;

uniform flat int u_PointLightCount;
uniform Material u_Material;

uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform DirectionalLight u_DirectionalLight;
uniform SpotLight u_Spotlight;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcDirLight(vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(vec3 normal, vec3 viewDir);

void main()
{
    vec3 normal = normalize(v_Normal);
    vec3 viewDir = normalize(u_CameraPosition - v_FragPosition);

    vec3 result = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < u_PointLightCount; i++)
        result += CalcPointLight(u_PointLights[i], normal, viewDir);
    
    // result += CalcDirLight(normal, viewDir);
    // result += CalcSpotLight(normal, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(v_FragPosition - light.Position);
    vec3 reflectDir = reflect(lightDir, normal);
    float diff = max(dot(normal, -lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);

    float dist = length(light.Position - v_FragPosition);
    float attenuation = 1.0 / (light.Constant
                             + light.Linear * dist
                             + light.Quadratic * (dist * dist));

    vec3 ambient  = light.Ambient  * 1.0  * vec3(texture(u_Material.Diffuse, v_TexCoord));
    vec3 diffuse  = light.Diffuse  * diff * vec3(texture(u_Material.Diffuse, v_TexCoord));
    vec3 specular = light.Specular * spec * vec3(texture(u_Material.Specular, v_TexCoord));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 light_dir = normalize(light.Direction);
    vec3 reflect_dir = reflect(light_dir, normal);
    float diff = max(dot(normal, -light_dir), 0.0);
    float spec = pow(max(dot(viewDir, reflect_dir), 0.0), u_Material.Shininess);

    vec3 ambient  = light.Ambient  * 1.0  * vec3(texture(u_Material.Diffuse, v_TexCoord));
    vec3 diffuse  = light.Diffuse  * diff * vec3(texture(u_Material.Diffuse, v_TexCoord));
    vec3 specular = light.Specular * spec * vec3(texture(u_Material.Specular, v_TexCoord));
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(vec3 normal, vec3 viewDir)
{
    float cutoff = cos(u_Spotlight.CutoffAngle);
    float outer = cos(u_Spotlight.OuterCutoffAngle);

    vec3 lightDir = normalize(v_FragPosition - u_Spotlight.Position.xyz);
    vec3 reflectDir = reflect(lightDir, normal);
    float diff = clamp(dot(normal, -lightDir), 0.0, 1.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);

    float theta = dot(-lightDir, -normalize(u_Spotlight.Direction.xyz));
    float epsilon = cutoff - outer;
    float intensity = clamp((theta - outer) / epsilon, 0.0, 1.0);

    vec3 ambient  = vec3(1.0, 1.0, 1.0);
    vec3 diffuse  = vec3(1.0, 1.0, 1.0);
    vec3 specular = vec3(1.0, 1.0, 1.0);

    vec3 ambient  = ambient  * 1.0  * vec3(texture(u_Material.Diffuse, v_TexCoord));
    vec3 diffuse  = diffuse  * diff * vec3(texture(u_Material.Diffuse, v_TexCoord));
    vec3 specular = specular * spec * vec3(texture(u_Material.Specular, v_TexCoord));

    return (diffuse + specular) * intensity;
}
