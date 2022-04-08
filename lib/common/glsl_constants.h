#pragma once

struct ShaderType{
    const char * vshader;
    const char * fshader;
};

namespace GLSL {
inline  constexpr  ShaderType PHONG_SHADER{
.vshader = R"(
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
    )",

.fshader = R"(
    #version 450
    #extension GL_ARB_separate_shader_objects : enable

    layout(binding = 1) uniform sampler2D texSampler;

    layout(location = 0) in vec3 fragColor;
    layout(location = 1) in vec2 fragTexCoord;
    layout(location = 2) in vec3 Normal;
    layout(location = 3) in vec3 FragPos;
    layout(location = 4) in vec3 viewPos;

    layout(location = 0) out vec4 outColor;

    void main() {

        vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
        vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);

        // ambient
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;

        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // specular
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor;  

        vec3 result = (ambient + diffuse + specular) * fragColor;
        outColor = vec4(result, 1.0);	
    }
    )"
};
inline  constexpr  ShaderType TEXTURE_SHADER{
    .vshader = R"(
        #version 450
        #extension GL_ARB_separate_shader_objects : enable

        layout(location = 0) in vec3 inPosition;
        layout(location = 1) in vec3 inColor;
        layout(location = 2) in vec3 inNormal;
        layout(location = 3) in vec2 inTexCoord;

        layout(location = 0) out vec3 fragColor;
        layout(location = 1) out vec2 fragTexCoord;

        layout(binding = 0) uniform UniformBufferObject {
            mat4 model;
            mat4 view;
            mat4 proj;
        } ubo;

        void main() {
            gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
            fragColor = inColor;
            fragTexCoord = inTexCoord;
        }
    )",
    .fshader = R"(
        #version 450
        #extension GL_ARB_separate_shader_objects : enable

        layout(binding = 1) uniform sampler2D texSampler;

        layout(location = 0) in vec3 fragColor;
        layout(location = 1) in vec2 fragTexCoord;

        layout(location = 0) out vec4 outColor;

        void main() {
            outColor = texture(texSampler, fragTexCoord);
        }
    )"
};

inline  constexpr  ShaderType NORMALMAP_SHADER{
    .vshader = R"(
        #version 450 
        #extension GL_ARB_separate_shader_objects : enable

        layout(location = 0) in vec3 inPosition;
        layout(location = 1) in vec3 inColor;
        layout(location = 2) in vec3 inNormal;
        layout(location = 3) in vec2 inTexCoord;

        layout(location = 0) out vec3 fragColor;
        layout(location = 1) out vec2 fragTexCoord;
        layout(location = 2) out vec3 Normal;

        layout(binding = 0) uniform UniformBufferObject {
            mat4 model;
            mat4 view;
            mat4 proj;
            vec3 viewPos;
        } ubo;

        void main() {
            mat4 modelView = ubo.view * ubo.model;
            mat4 normalMatrix = transpose(inverse(modelView));
            Normal = normalize(vec3(normalMatrix * vec4(inNormal, 1.0)));
            gl_Position = ubo.proj * modelView * vec4( inPosition,1.0);
        }
    )",
    .fshader = R"(
        #version 450

        layout(location = 0) in vec3 fragColor;
        layout(location = 1) in vec2 fragTexCoord;
        layout(location = 2) in vec3 Normal;

        layout(location = 0) out vec4 outColor;

        void main(){
            outColor = vec4(Normal, 1.0);
         }
    )"
};

inline  constexpr  ShaderType DUMMY_SHADER{
    .vshader = R"(
        #version 450 
        layout(location = 0) in vec3 inPosition; 
        void main(){ vec4(inPosition, 1.0); }
    )",
    .fshader = R"(
        #version 450
        layout(location = 0) out vec4 outColor;
        void main(){ }
    )"
};

} // GLSL namespace
