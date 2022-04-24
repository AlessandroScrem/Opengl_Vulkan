#pragma once
// lib common
#include <mytypes.hpp>
#include <glsl_constants.h>
// lib
#include <GL/glew.h>
#include <glm/glm.hpp>
// std
#include <vector>

class OpenglShader
{
public:
    OpenglShader(GLSL::ShaderType type = GLSL::PHONG) 
        : shaderType{type}
    {
        SPDLOG_DEBUG("constructor"); 
        buildShaders();
        createUniformBlockBinding();
    }

    ~OpenglShader(){
        SPDLOG_DEBUG("destructor"); 
        glDeleteProgram(shaderProgram);
    }
    
    void use(){glUseProgram(shaderProgram);}
    
    void setVec1(const std::string &name, const float value) const
    { 
        glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()),  value); 
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z); 
    }

private:
    void compile(GLuint shader, std::vector<char> &glsl, GLenum  kind);
    void link();
    void buildShaders();

    void createUniformBlockBinding(){
        uniformBlockIndex    = glGetUniformBlockIndex(shaderProgram, "ubo");        
        glUniformBlockBinding(shaderProgram,    uniformBlockIndex, 0);
    }

    GLSL::ShaderType shaderType;
    unsigned int shaderProgram;
    unsigned int uniformBlockIndex; 
};

