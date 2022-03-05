#pragma once


enum class ShaderType{
    Texture = 0,
    Phong,

    COUNT
};

struct Shader{
    std::string vshader{};
    std::string fshader{};
};

static const char * vertex_texture = R"(
        #version 450 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aCol;
        layout (location = 2) in vec2 aTexCoord;
        
        layout (std140) uniform UniformBufferObject {
           mat4 model;
           mat4 view;
           mat4 proj;
        }ubo;

        out vec3 ourColor;
        out vec2 TexCoord;
        void main()
        {
           ourColor = aCol;
           gl_Position = ubo.proj * ubo.view * ubo.model * vec4(aPos, 1.0);
           TexCoord = aTexCoord;
        }
        )";

static const char *fragment_texture = R"(
        #version 450 core

        in vec3 ourColor;
        in vec2 TexCoord;
        out vec4 FragColor;

        uniform sampler2D ourTexture;

        void main()
        {
           FragColor = texture(ourTexture, TexCoord);
        }
        )";

static const char * vertex_phong = R"(
        #version 450 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aCol;
        layout (location = 2) in vec3 aNorm;
        
        layout (std140) uniform UniformBufferObject {
           mat4 model;
           mat4 view;
           mat4 proj;
        }ubo;

        out vec3 ourColor;
        void main()
        {
           ourColor = aCol;
           gl_Position = ubo.proj * ubo.view * ubo.model * vec4(aPos, 1.0);
        }
        )";

static const char *fragment_phong = R"(
        #version 450 core

        in vec3 ourColor;
        out vec4 FragColor;

        void main()
        {
           FragColor = vec4(ourColor, 1.0f);
        }
        )";

