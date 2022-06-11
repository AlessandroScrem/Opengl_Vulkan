#pragma once
//common
#include "glsl_constants.h"
#include "model.hpp"
//libs
#include <glm/glm.hpp>
//std
#include <string>
#include <memory>

struct Shader;
struct RenderObject;
class Model;

class ShaderBuilder{
public:
    virtual ~ShaderBuilder(){}
    virtual ShaderBuilder& type(GLSL::ShaderType id)  = 0;
    virtual ShaderBuilder& addTexture(std::string image, uint32_t binding)  = 0;
    virtual ShaderBuilder& setPolygonMode(GLSL::PolygonMode mode)  = 0;
    virtual std::unique_ptr<Shader> build()  = 0;
    virtual ShaderBuilder& Reset() = 0;
};

struct Shader{
private:
    static inline std::unique_ptr<ShaderBuilder> builder_ = nullptr;
public:
    static inline void addBuilder(std::unique_ptr<ShaderBuilder> builder ){ builder_ = std::move(builder); };
    static inline ShaderBuilder& make() { return builder_->Reset(); }

    virtual  ~Shader() {}

};


class ObjectBuilder{
public:
    virtual ~ObjectBuilder(){}
    virtual std::unique_ptr<RenderObject> build(Model &model, std::string shader)  = 0;
    virtual ObjectBuilder& Reset() = 0;
};

struct RenderObject  
{
private:
    static inline std::unique_ptr<ObjectBuilder> builder_ = nullptr;
public:
    virtual  ~RenderObject() {}
    static inline void addBuilder(std::unique_ptr<ObjectBuilder> builder ){ builder_ = std::move(builder); };	
    static inline ObjectBuilder& make() { return builder_->Reset();}

    std::string shader;

    std::string objName;
    Node objNode{};
};
