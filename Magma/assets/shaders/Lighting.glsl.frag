#version 460 core

#define MAX_POINT_LIGHTS 50
#define MAX_SPOT_LIGHTS 50

struct DirectionalLight {
    vec4 Position;
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    vec4 Direction;
};

struct PointLight {
    vec4 Position;
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;

    float Constant;
    float Linear;
    float Quadratic;
    float BloomStrength;
};

struct Spotlight {
    vec4 Position;
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    vec4 Direction;

    float CutoffAngle;
    float OuterCutoffAngle;
};

struct Material {
    sampler2D Diffuse;
    sampler2D Specular;
    vec4 DiffuseColor;
    vec4 SpecularColor;
    int IsTextured;
    float Shininess;
};

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

layout(location = 1) uniform int u_DirectionalLight;
layout(location = 2) uniform int u_PointLightCount;
layout(location = 3) uniform int u_SpotlightCount;
layout(location = 4) uniform vec3 u_CameraPosition;
layout(location = 5) uniform Material u_Material;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoords;

layout(location = 0) out vec4 FragColor;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotlight(Spotlight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 result = vec3(0.0, 0.0, 0.0);
    vec3 normal = normalize(v_Normal);
    vec3 viewDir = normalize(u_CameraPosition - v_Position);

    for(int i = 0; i < u_PointLightCount; i++)
        result += CalcPointLight(u_PointLights.Buffer[i], normal, viewDir);
    for(int i = 0; i < u_SpotlightCount; i++)
        result += CalcSpotlight(u_Spotlights.Buffer[i], normal, viewDir);

    vec3 color;
    if(u_Material.IsTextured == 1)
        color = texture(u_Material.Diffuse, v_TexCoords.xy).rgb;
    else
        color = u_Material.DiffuseColor.rgb;

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.Direction.xyz);
    vec3 reflectDir = reflect(lightDir, normal);
    float diff = max(dot(normal, -lightDir), 0.0);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 color;
    if(u_Material.IsTextured == 1)
        color = texture(u_Material.Diffuse, v_TexCoords.xy).rgb;
    else
        color = u_Material.DiffuseColor.rgb;

    vec3 ambient  = light.Ambient.xyz  * 1.0  * color;
    vec3 diffuse  = light.Diffuse.xyz  * diff * color;
    vec3 specular = light.Specular.xyz * spec * vec3(texture(u_Material.Specular, v_TexCoords.xy));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(v_Position - light.Position.xyz);
    vec3 reflectDir = reflect(lightDir, normal);
    float diff = max(dot(normal, -lightDir), 0.0);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float dist = length(light.Position.xyz - v_Position);

    if(light.Constant == 0.0 && light.Linear == 0.0 && light.Quadratic == 0.0)
        return vec3(0.0);

    float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));

    vec3 color;
    if(u_Material.IsTextured == 1)
        color = texture(u_Material.Diffuse, v_TexCoords.xy).rgb;
    else
        color = u_Material.DiffuseColor.rgb;

    vec3 ambient  = light.Ambient.xyz  * 1.0  * color;
    vec3 diffuse  = light.Diffuse.xyz  * diff * color;
    vec3 specular = light.Specular.xyz * spec * vec3(texture(u_Material.Specular, v_TexCoords.xy));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotlight(Spotlight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(v_Position - light.Position.xyz);
    vec3 reflectDir = reflect(lightDir, normal);
    float diff = clamp(dot(normal, -lightDir), 0.0, 1.0);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float cutoff = cos(light.CutoffAngle);
    float outer = cos(light.OuterCutoffAngle);

    float theta = dot(-lightDir, -normalize(light.Direction.xyz));
    float epsilon = cutoff - outer;
    float intensity = clamp((theta - outer) / epsilon, 0.0, 1.0);

    vec3 color;
    if(u_Material.IsTextured == 1)
        color = texture(u_Material.Diffuse, v_TexCoords.xy).rgb;
    else
        color = u_Material.DiffuseColor.rgb;

    vec3 ambient  = light.Ambient.xyz  * 1.0 * color;
    vec3 diffuse  = light.Diffuse.xyz  * diff * color;
    vec3 specular = light.Specular.xyz * spec * vec3(texture(u_Material.Specular, v_TexCoords.xy));

    return (ambient + diffuse + specular) * intensity;
}
