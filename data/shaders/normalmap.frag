#version 450

layout(location = 0) in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

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

vec3 gamma(vec3 color){

    #ifdef VULKAN
        color = vec3ToSrgb(color);
    #endif
    return color;   
}
void main(){
    // use normal as color
    vec3 color = fs_in.Normal;
    color = gamma(color);

    outColor = vec4(color, 1.0);

}