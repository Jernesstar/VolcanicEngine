#version 450 core

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec2 v_TextureCoordinate;
layout(location = 2) in vec3 v_Normal;

layout(binding = 0) uniform sampler2D u_Diffuse;
layout(binding = 1) uniform sampler2D u_Specular;
layout(binding = 1) uniform sampler2D u_Roughness;
uniform vec3 u_CameraPosition;
uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;

out vec4 FragColor;

void main()
{
    float shininess = 64;
    float amb = 0.4;

    vec3 diffuse_color = texture(u_Diffuse, v_TextureCoordinate).xyz;
    vec3 specular_color = texture(u_Specular, v_TextureCoordinate).xyz;
    vec3 roughness = texture(u_Roughness, v_TextureCoordinate).xyz;

    // vec3 ambient = u_LightColor * ambient_color;

    vec3 normal = normalize(v_Normal);
    vec3 light_dir = normalize(v_Position - u_LightPosition);
    float diff = max(dot(normal, -light_dir), 0.0);

    vec3 view_dir = normalize(u_CameraPosition - v_Position);
    vec3 reflect_dir = reflect(light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);

    vec3 ambient  = u_LightColor * diffuse_color;
    vec3 diffuse  = u_LightColor * (diff * diffuse_color);
    vec3 specular = u_LightColor * (spec * specular_color);

    vec3 result = diffuse + specular;
    FragColor = vec4(result, 1.0);
}