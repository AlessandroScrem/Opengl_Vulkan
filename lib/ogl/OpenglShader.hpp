#pragma once
// lib common
#include <mytypes.hpp>
#include <glsl_constants.h>
// lib
#include <GL/glew.h>
// std
#include <array>
#include <map>

class OpenglShader
{
private:
    enum class ShaderSourceType{
        VertexShader,
        FragmentShader
    };

    class ShaderSource{
    public:
        ShaderSource(GLSL::ShaderType shaderType, ShaderSourceType type ) : type{type}{ 
            SPDLOG_TRACE("{} shader constructor", shadername); 
            compile(shaderType);
        }

        ~ShaderSource(){
            SPDLOG_TRACE("{} shader destructor", shadername); 
            glDeleteShader(shader);
        }

        unsigned int get() const {return shader;}

    private:
        void compile(GLSL::ShaderType shaderType){ 

            std::string source;     

            if(type == ShaderSourceType::VertexShader){
                auto glsl = GLSL::readFile(GLSL::getname(shaderType) + ".vert");
                source = std::string(glsl.begin(), glsl.end());
                shader = glCreateShader(GL_VERTEX_SHADER);
                shadername = "VERTEX";
            }

            if(type == ShaderSourceType::FragmentShader){
                auto glsl = GLSL::readFile(GLSL::getname(shaderType) + ".frag");
                source = std::string(glsl.begin(), glsl.end());
                shader = glCreateShader(GL_FRAGMENT_SHADER);
                shadername = "FRAGMENT";
            }

            // build and compile our shader program
            const char * ShaderCode = source.c_str();
            glShaderSource(shader, 1, &ShaderCode, NULL);
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
        ShaderSourceType type;
        std::string shadername; 
    };

public:
    OpenglShader(GLSL::ShaderType type = GLSL::PHONG){
        SPDLOG_DEBUG("constructor"); 
        buildShaders(type);
        createUniformBlockBinding();
    }

    ~OpenglShader(){
        SPDLOG_DEBUG("destructor"); 
        glDeleteProgram(shaderProgram);
    }
    
    void use(){glUseProgram(shaderProgram);}
    
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z); 
    }

private:
    template <typename Container>
    void link(const Container &shaders){
        
        for ( auto & shader : shaders){
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

    void buildShaders(GLSL::ShaderType shaderType){

        std::array<ShaderSource, 2> shaders{ {
            ShaderSource(shaderType, ShaderSourceType::VertexShader),
            ShaderSource(shaderType, ShaderSourceType::FragmentShader)
        } };

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
};

