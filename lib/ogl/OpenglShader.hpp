#pragma once
// lib common
#include <baseclass.hpp>
#include <mytypes.hpp>
#include <glsl_constants.h>
// lib
#include <GL/glew.h>
#include <glm/glm.hpp>
// std


class OpenglUbo;
class OpenglImage;

class OpenglShader : public Shader
{
struct ShaderBindigs{
    std::unique_ptr<OpenglImage> image;
    std::unique_ptr<OpenglUbo>   ubo;
    uint32_t    imageBindig = 0;
    uint32_t    uboBindig   = 0; 

}shaderBindings;

public:
    OpenglShader(GLSL::ShaderType type = GLSL::PHONG);
    ~OpenglShader();

    void addTexture(std::string imagepath, uint32_t binding);
    void addUbo(uint32_t binding);
    void addConstant(uint32_t binding);
    void setPolygonMode(GLenum mode);
    void setTopology(GLenum mode);
    void buid();  
    void use();

    OpenglUbo & getUbo();
    GLenum getTopology(){return topology;}
    
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

    GLSL::ShaderType shaderType;
    unsigned int shaderProgram;

    GLenum  polygonMode = GL_FILL; 
    GLenum  topology = GL_TRIANGLES;
};

