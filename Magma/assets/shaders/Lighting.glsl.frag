#version 450 core

#define MAX_POINT_LIGHTS 50
#define MAX_SPOT_LIGHTS 50

struct DirectionalLight {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular; // 12

    vec3 Direction;
    float _dummy;
};

struct PointLight {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular; // 12

    float Constant;
    float Linear;
    float Quadratic;
    float _dummy;
};

struct Spotlight {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular; // 12

    vec3 Direction;
    float CutoffAngle;

    float OuterCutoffAngle;
    vec3 _dummy;
};

struct Material {
    sampler2D Diffuse;
    sampler2D Specular;
    vec4 DiffuseColor;
    vec4 SpecularColor;
    int IsTextured;
    float Shininess;
};

uniform vec3 u_CameraPosition;

uniform int u_DirectionalLightCount;
uniform int u_PointLightCount;
uniform int u_SpotlightCount;

layout(std140, binding = 0) uniform DirectionalLights
{
    DirectionalLight Buffer[1];
} u_DirectionalLights;

layout(std140, binding = 1) uniform PointLights
{
    PointLight Buffer[MAX_POINT_LIGHTS];
} u_PointLights;

layout(std140, binding = 2) uniform Spotlights
{
    Spotlight Buffer[MAX_SPOT_LIGHTS];
} u_Spotlights;

uniform Material u_Material;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotlight(Spotlight light, vec3 normal, vec3 viewDir);

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoords;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(v_Normal);
    vec3 viewDir = normalize(u_CameraPosition - v_Position);

    vec3 result = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < u_PointLightCount; i++)
        result += CalcPointLight(u_PointLights.Buffer[i], normal, viewDir);
    for(int i = 0; i < u_SpotlightCount; i++)
        result += CalcSpotlight(u_Spotlights.Buffer[i], normal, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.Direction.xyz);
    vec3 reflectDir = reflect(lightDir, normal);
    float diff = max(dot(normal, -lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);

    vec3 ambient  = light.Ambient.xyz  * 1.0  * vec3(texture(u_Material.Diffuse, v_TexCoords.xy));
    vec3 diffuse  = light.Diffuse.xyz  * diff * vec3(texture(u_Material.Diffuse, v_TexCoords.xy));
    vec3 specular = light.Specular.xyz * spec * vec3(texture(u_Material.Specular, v_TexCoords.xy));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(v_Position - light.Position);
    vec3 reflectDir = reflect(lightDir, normal);
    float diff = max(dot(normal, -lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);

    float dist = length(light.Position - v_Position);
    float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));

    vec3 color;
    if(u_Material.IsTextured == 1)
        color = texture(u_Material.Diffuse, v_TexCoords.xy).rgb;
    else
        color = u_Material.DiffuseColor.rgb;

    vec3 ambient  = light.Ambient.xyz  * 1.0  * vec3(texture(u_Material.Diffuse, v_TexCoords.xy));
    vec3 diffuse  = light.Diffuse.xyz  * diff * vec3(texture(u_Material.Diffuse, v_TexCoords.xy));
    vec3 specular = light.Specular.xyz * spec * vec3(texture(u_Material.Specular, v_TexCoords.xy));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotlight(Spotlight light, vec3 normal, vec3 viewDir)
{
    float cutoff = cos(light.CutoffAngle);
    float outer = cos(light.OuterCutoffAngle);

    vec3 lightDir = normalize(v_Position - light.Position);
    vec3 reflectDir = reflect(lightDir, normal);
    float diff = clamp(dot(normal, -lightDir), 0.0, 1.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);

    float theta = dot(-lightDir, -normalize(light.Direction.xyz));
    float epsilon = cutoff - outer;
    float intensity = clamp((theta - outer) / epsilon, 0.0, 1.0);

    vec3 color;
    if(u_Material.IsTextured == 1)
        color = texture(u_Material.Diffuse, v_TexCoords.xy).rgb;
    else
        color = u_Material.DiffuseColor.rgb;

    vec3 ambient  = light.Ambient.xyz * 1.0 * color;
    vec3 diffuse  = light.Diffuse.xyz * diff * color;
    vec3 specular = light.Specular.xyz * spec * vec3(texture(u_Material.Specular, v_TexCoords.xy));

    return (ambient + diffuse + specular) * intensity;
}
