#pragma once

#include "ngn_command.hpp"
//common lib
#include <mytypes.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <glsl_constants.h>
#include <multiplatform_input.hpp>
//std
#include <array>
#include <memory>

struct Color
{
    float red = 0.f;;
    float green = 0.f;;
    float blue = 0.f;
    float alpha = 1.0f;
};

class Engine
{    
public:
    Engine();
    virtual ~Engine();
       
    virtual void run() = 0;
    virtual void setWindowMessage(std::string msg) =0;

   static std::unique_ptr<Engine> create(EngineType type);

protected:
    void updateEvents();

    ngn::MultiplatformInput input_{};
    EngineType engine_type{};
    const  ShaderType &phong_glslShader = GLSL::PHONG_SHADER;
    const  ShaderType &tex_glslShader = GLSL::TEXTURE_SHADER;

    // default model space is Yup   
    std::vector<Model> _models{};

    
    Color background{};
    Camera ourCamera{};

    size_t _model_index{0};

private:
    void MapActions();
    void loadModels();
    
    std::unordered_map<std::string, std::unique_ptr<ngn::Command>> commands_{};
    bool shouldupdate = false;


    static std::unique_ptr<Engine> makeVulkan();
    static std::unique_ptr<Engine> makeOpengl();
};

