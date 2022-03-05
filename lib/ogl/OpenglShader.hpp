#pragma once
#include "shader_constants.h"
// lib
#include <GL/glew.h>
#include <spdlog/spdlog.h>
// std
#include <initializer_list>
#include <array>
#include <map>

class OpenglShader
{
private:

    inline static std::map<ShaderType, Shader> shaders{ 
        { ShaderType::Texture, Shader(vertex_texture, fragment_texture) },  
        { ShaderType::Phong, Shader(vertex_phong, fragment_phong) },  
    };

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
    OpenglShader(ShaderType type = ShaderType::Texture){
        SPDLOG_TRACE("constructor"); 
        buildShaders(type);
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

    void buildShaders(ShaderType type){

        auto shader = shaders.at(type);

        std::array<ShaderSource, 2> shaders{ {
            ShaderSource(shader.vshader.c_str(), ShaderSourceType::VertexShader),
            ShaderSource(shader.fshader.c_str(), ShaderSourceType::FragmentShader)
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

