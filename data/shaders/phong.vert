#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 Normal;
layout(location = 3) out vec3 FragPos;
layout(location = 4) out vec3 viewPos;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} ubo;

void main() {
    fragColor = inColor;
    FragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    Normal = mat3(transpose(inverse(ubo.model))) * inNormal; 
    gl_Position = ubo.proj * ubo.view * vec4(FragPos, 1.0);
    fragTexCoord = inTexCoord;
    viewPos = ubo.viewPos;
}