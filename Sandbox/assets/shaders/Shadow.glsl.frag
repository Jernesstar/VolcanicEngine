#version 450 core

uniform vec3 u_CameraPosition;
uniform vec3 u_LightPosition;

layout(binding = 0) uniform sampler2D u_Diffuse;
layout(binding = 1) uniform sampler2D u_ShadowMap;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec4 TexCoords;
layout(location = 3) in vec4 FragPosLightSpace;

out vec4 FragColor;

float ShadowCalculation()
{
    // perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    vec3 lightDir = normalize(FragPos - u_LightPosition);
    float bias = max(0.0001 * (1.0 - dot(Normal, lightDir)), 0.0001);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 color = TexCoords.xyz;
    if(TexCoords.a == 0.0)
        color = texture(u_Diffuse, TexCoords.xy).rgb;
    else
    {
        FragColor = vec4(color, 1.0);
        return;
    }

    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.15 * lightColor;
    // diffuse
    vec3 lightDir = normalize(FragPos - u_LightPosition);
    float diff = max(dot(-lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(FragPos - u_CameraPosition);
    float spec = 0.0;
    vec3 halfwayDir = normalize((-lightDir) + (-viewDir));
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor; 
    // calculate shadow
    float shadow = ShadowCalculation();
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}
