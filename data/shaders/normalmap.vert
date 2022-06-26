#version 450 
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 FragCol;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
    vec3 drawLines;
} ubo;

layout (binding = 2) uniform UboInstance {
	mat4 model; 
} uboInstance;


void main() {
    mat4 modelView = ubo.view * uboInstance.model;
    mat4 normalMatrix = transpose(inverse(modelView));
    vec3 Normal = normalize(vec3(normalMatrix * vec4(inNormal, 1.0)));
        
    // use normal as color
    FragCol = ubo.drawLines.x > 0 ? vec3(0.0) : Normal;

    gl_Position = ubo.proj * modelView * vec4( inPosition,1.0);
}