#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 Normal;

layout(location = 0) out vec4 outColor;

float floatToSrgb(float value){
    const float inv_12_92 = 0.0773993808;
    return value <= 0.04045
        ? value * inv_12_92
        : pow((value + 0.055) / 1.055, 2.4);
}
vec3 vec3ToSrgb(vec3 value){
    return vec3(floatToSrgb(value.x), floatToSrgb(value.y), floatToSrgb(value.z));
}

void main(){
    #ifdef VULKAN
        outColor = vec4(vec3ToSrgb(Normal), 1.0);
    #else
        outColor = vec4(Normal, 1.0);
    #endif
    }