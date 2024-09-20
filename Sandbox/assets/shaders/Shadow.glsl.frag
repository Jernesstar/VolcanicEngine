#version 450 core

uniform sampler2D u_Diffuse;
uniform sampler2D u_ShadowMap;

uniform vec3 u_LightPosition;
uniform vec3 u_CameraPosition;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    // vec3 lightDir = normalize(fs_in.FragPos - u_LightPosition);
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 color;
    if(fs_in.TexCoords.a == 0.0)
        color = fs_in.TexCoords.xyz;
    else
        color = texture(u_Diffuse, fs_in.TexCoords.xy).rgb;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.15 * lightColor;
    // diffuse
    vec3 lightDir = normalize(u_LightPosition - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(u_CameraPosition - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor; 
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}