#version 450 
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 FragCol;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} ubo;

 
void main() {
    FragCol = inColor;

    mat4 view_no_traslation = ubo.view;
    view_no_traslation[3].x =  0; // zero x translations
    view_no_traslation[3].y =  0; // zero y translations
    view_no_traslation[3].z = -1; // move away from camera
    view_no_traslation[3].w =  1;

    vec3 position = inPosition * 40; // resize obj to 40 pixel

    gl_Position = ubo.proj * view_no_traslation * vec4( position,1.0);
}
 