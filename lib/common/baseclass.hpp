#pragma once
#include <string>
#include <glm/glm.hpp>

struct Shader{
    virtual  ~Shader() {}

};

struct VertexBuffer{
    virtual  ~VertexBuffer() {}
};

struct RenderObject  
{
    virtual  ~RenderObject() {}	

    std::string shader;
    glm::mat4 model;
};