#version 450

layout(location = 0) in vec3 FragCol;

layout(location = 0) out vec4 outColor;

void main(){
    outColor = vec4(FragCol, 1.0);
}