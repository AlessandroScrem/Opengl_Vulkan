#version 450

layout(location = 0) in vec3 FragCol;

layout(location = 0) out vec4 outColor;


void main(){
    
    vec3 color = FragCol;

    outColor = vec4(color, 1.0);

}