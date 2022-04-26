#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in VS_OUT {
    vec3 fragColor;
    vec2 fragTexCoord;
    vec3 Normal;
    vec3 FragPos;
    vec3 viewPos;
} fs_in;

layout(location = 0) out vec4 outColor;

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
    // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(fs_in.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // simple attenuation
    float max_distance = 1.5;
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (distance);
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    return diffuse + specular;
}



vec3 Phong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor){

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(fs_in.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    return ambient + diffuse + specular;
}

float floatToSrgb(float value){
    const float inv_12_92 = 0.0773993808;
    return value <= 0.04045
        ? value * inv_12_92
        : pow((value + 0.055) / 1.055, 2.4);
}
vec3 vec3ToSrgb(vec3 value){
    return vec3(floatToSrgb(value.x), floatToSrgb(value.y), floatToSrgb(value.z));
}

vec3 gamma(vec3 color){

    #ifdef VULKAN
        color = vec3ToSrgb(color);
    #endif
    return color;   
}

void main(){

    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);

    vec3 lighting = vec3(0.0);
    vec3 color = fs_in.fragColor;

    lighting += BlinnPhong(fs_in.Normal, fs_in.FragPos, lightPos, lightColor);
    color *=  lighting;
    color = gamma(color);

    outColor = vec4(color, 1.0);
}