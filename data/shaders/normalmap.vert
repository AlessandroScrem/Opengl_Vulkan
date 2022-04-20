#version 450 
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} ubo;

void main() {
    mat4 modelView = ubo.view * ubo.model;
    mat4 normalMatrix = transpose(inverse(modelView));
    vs_out.Normal = normalize(vec3(normalMatrix * vec4(inNormal, 1.0)));
    gl_Position = ubo.proj * modelView * vec4( inPosition,1.0);
}