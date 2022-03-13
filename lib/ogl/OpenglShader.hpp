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
        ShaderSource(const char *source , ShaderSourceType type ) : source{source}, type{type}{ 
            SPDLOG_TRACE("{} shader constructor", shadername); 
            compile();
        }

        ~ShaderSource(){
            SPDLOG_TRACE("{} shader destructor", shadername); 
            glDeleteShader(shader);
        }

        unsigned int get() const {return shader;}

    private:
        void compile(){       

            if(type == ShaderSourceType::VertexShader){
                shader = glCreateShader(GL_VERTEX_SHADER);
                shadername = "VERTEX";
            }

            if(type == ShaderSourceType::FragmentShader){
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
        ShaderSourceType type;
        std::string shadername; 
    };

public:
    OpenglShader(ShaderType type = GLSL::TEXTURE_SHADER){
        SPDLOG_TRACE("constructor"); 
        buildShaders(type);
        createUniformBlockBinding();
    }

    ~OpenglShader(){
        SPDLOG_TRACE("destructor"); 
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

    void buildShaders(ShaderType shader){

        std::array<ShaderSource, 2> shaders{ {
            ShaderSource(shader.vshader, ShaderSourceType::VertexShader),
            ShaderSource(shader.fshader, ShaderSourceType::FragmentShader)
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

