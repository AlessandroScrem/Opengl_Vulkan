// lib
#include <GL/glew.h>
#include <spdlog/spdlog.h>
// std
#include <initializer_list>
#include <array>

class OpenglShader
{
private:
    enum class ShaderType{
        VertexShader,
        FragmentShader
    };

    class ShaderSource{
    public:
        ShaderSource(const char *source , ShaderType type ) : source{source}, type{type}{ 
            SPDLOG_TRACE("{} shader constructor", shadername); 
            compile();
        }

        ~ShaderSource(){
            SPDLOG_TRACE("{} shader destructor", shadername); 
            glDeleteShader(shader);
        }

        unsigned int get(){return shader;}

    private:
        void compile(){       

            if(type == ShaderType::VertexShader){
                shader = glCreateShader(GL_VERTEX_SHADER);
                shadername = "VERTEX";
            }

            if(type == ShaderType::FragmentShader){
                shader = glCreateShader(GL_FRAGMENT_SHADER);
                shadername = "FRAGMENT";
            }

            // build and compile our shader program
            glShaderSource(shader, 1, &source, NULL);
            glCompileShader(shader);

            // check for shader compile errors
            int success;
            char infoLog[512];
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 512, NULL, infoLog);
                spdlog::error("ERROR::SHADER::{}::COMPILATION_FAILED\n {}", shadername, infoLog);
            }
        }

        unsigned int shader;
        const char *source;
        ShaderType type;
        std::string shadername; 
    };

public:
    OpenglShader(){
        SPDLOG_TRACE("constructor"); 
        buildShaders();
        createUniformBlockBinding();
    }

    ~OpenglShader(){
        SPDLOG_TRACE("destructor"); 
        glDeleteProgram(shaderProgram);
    }
    
    void use(){glUseProgram(shaderProgram);}
    
    // void setMat4(const std::string &name, const glm::mat4 &mat) const
    // {
    //     glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    // }

private:
    template <typename Container>
    void link(const Container &shaders){
        
        for (auto shader : shaders){
            glAttachShader(shaderProgram, shader.get() );
        }

        glLinkProgram(shaderProgram);
        
        // check for linking errors
        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            spdlog::error("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}", infoLog);
        }

    }


    void buildShaders(){

        std::array<ShaderSource, 2> shaders{
            ShaderSource(vertexShaderSource, ShaderType::VertexShader),
            ShaderSource(fragmentShaderSource, ShaderType::FragmentShader)
        };

        shaderProgram = glCreateProgram();  
        // link shaders
        link(shaders);
    }

    void createUniformBlockBinding(){
        uniformBlockIndex    = glGetUniformBlockIndex(shaderProgram, "ubo");        
        glUniformBlockBinding(shaderProgram,    uniformBlockIndex, 0);
    }

    unsigned int shaderProgram;
    unsigned int uniformBlockIndex;

    const char *vertexShaderSource = R"(
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

    const char *fragmentShaderSource = R"(
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

};

