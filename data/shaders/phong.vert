#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out VS_OUT {
    vec3 fragColor;
    vec2 fragTexCoord;
    vec3 Normal;
    vec3 FragPos;
    vec3 viewPos;
    vec3 drawLines;
} vs_out;


layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
    vec3 drawLines;
} ubo;

// layout (binding = 1) uniform UboInstance {
// 	mat4 model; 
// } uboInstance;

void main() {
    vs_out.fragTexCoord = inTexCoord;
    vs_out.fragColor = inColor;
    vs_out.viewPos = ubo.viewPos;
    vs_out.drawLines = ubo.drawLines;

    vs_out.FragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    vs_out.Normal = mat3(transpose(inverse(ubo.model))) * inNormal; 

    gl_Position = ubo.proj * ubo.view * vec4(vs_out.FragPos, 1.0);
}