#pragma once
// lib common
#include <baseclass.hpp>
#include <mytypes.hpp>
#include <vertex.h>
#include <glsl_constants.h>
// lib
#include <GL/glew.h>
#include <glm/glm.hpp>
// std
#include <map>



class OpenglUbo;
class OpenglImage;
class OpenglShader;

class OpenglShaderBuilder : public ShaderBuilder{
private:
    std::unique_ptr<OpenglShader> shader;

public:

    ShaderBuilder& Reset() override;
    ShaderBuilder& type(GLSL::ShaderType id)  override;
    ShaderBuilder& addTexture(std::string image, uint32_t binding)  override;
    ShaderBuilder& setPolygonMode(GLSL::PolygonMode mode) override;
    std::unique_ptr<Shader> build() override;

};

class OpenglShader : public Shader
{
struct ShaderBindigs{
    std::map<uint32_t, std::unique_ptr<OpenglImage> > image;
    std::map<uint32_t, std::unique_ptr<OpenglUbo> >   ubo;

}shaderBindings;

public:
    friend class OpenglShaderBuilder;

    OpenglShader(GLSL::ShaderType type = GLSL::PHONG);
    ~OpenglShader();

    void buid();  
    void bind();
    void updateUbo(UniformBufferObject & mvp);
    void addConstant(uint32_t binding);

    GLenum getTopology(){return topology;}
    

private:
    void createGlobalUbo();
    void buildShaders();
    void compile(GLuint shader, std::vector<char> &glsl, GLenum  kind);
    void link();
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

    GLSL::ShaderType shaderType;
    unsigned int shaderProgram;
    const uint32_t globalUboBinding = 0;

    bool prepared = false;

    GLenum  polygonMode = GL_FILL; 
    GLenum  topology = GL_TRIANGLES;
};

