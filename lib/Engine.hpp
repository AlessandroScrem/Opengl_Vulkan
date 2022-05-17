#pragma once

#include "ngn_command.hpp"
//common lib
#include <baseclass.hpp>
#include <mytypes.hpp>
#include <camera.hpp>
#include <multiplatform_input.hpp>
//std
#include <vector>
#include <memory>


struct Color
{
    float red = 0.2f;;
    float green = 0.3f;;
    float blue = 0.3f;
    float alpha = 1.0f;
};

class Window;

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

    std::unordered_map< std::string, std::unique_ptr<Shader> > _shaders;
    std::unordered_map< std::string, std::unique_ptr<RenderObject> > _fixed_objects;
    std::vector< std::unique_ptr<RenderObject> > _renderables;
    
    Color background{};
    Camera ourCamera{};
    // std::unique_ptr<Window> window;

    size_t _model_index{0};

private:
    void MapActions();
    
    std::unordered_map<std::string, std::unique_ptr<ngn::Command>> commands_{};
    bool shouldupdate = false;

    static std::unique_ptr<Engine> makeVulkan();
    static std::unique_ptr<Engine> makeOpengl();
};

