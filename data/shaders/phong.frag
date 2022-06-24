#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in VS_OUT {
    vec3 fragColor;
    vec2 fragTexCoord;
    vec3 Normal;
    vec3 FragPos;
    vec3 viewPos;
    vec3 drawLines;
} fs_in;

layout(location = 0) out vec4 outColor;

vec3 Phong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 fragColor){

    // ambient
    vec3 lightAmbient = vec3(0.2, 0.2, 0.2);
    vec3 ambient = lightAmbient * fragColor;

    // diffuse 
    vec3 lightDiffuse = vec3(0.5f, 0.5f, 0.5f);
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff * fragColor;

    // specular
    vec3 lightSpecular = vec3(1.0f, 1.0f, 1.0f);
    float shininess = 32.0;
    vec3 viewDir = normalize(fs_in.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = lightSpecular * spec * fragColor;  

    return ambient + diffuse + specular;
}

void main(){

    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);

    // light from point of view
    // vec3 lightPos = fs_in.viewPos;

    vec3 color = Phong(fs_in.Normal, fs_in.FragPos, lightPos, fs_in.fragColor);
    // if wireframe color is black    
    color = fs_in.drawLines.x > 0 ? vec3(0.0) : color;

    outColor = vec4(color, 1.0);
}