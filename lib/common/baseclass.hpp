#pragma once
//common
#include "glsl_constants.h"
//libs
#include <glm/glm.hpp>
//std
#include <string>
#include <memory>

struct Shader;

class Builder{
public:
    virtual ~Builder(){}
    virtual Builder& type(GLSL::ShaderType id)  =0;
    virtual Builder& addTexture(std::string image, uint32_t binding)  =0;
    virtual Builder& setPolygonMode(uint32_t mode)  =0;
    virtual std::unique_ptr<Shader> build()  =0;
};

struct Shader{
private:
    static inline std::unique_ptr<Builder> builder_ = nullptr;
public:
    static inline void addBuilder(std::unique_ptr<Builder> builder ){ builder_ = std::move(builder); };
    static inline Builder& make() {return *builder_;}

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