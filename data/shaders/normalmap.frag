#version 450

layout(location = 0) in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

layout(location = 0) out vec4 outColor;

void main(){
    // use normal as color
    vec3 color = fs_in.Normal;

    outColor = vec4(color, 1.0);

}