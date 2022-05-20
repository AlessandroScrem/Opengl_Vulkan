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
    Engine(EngineType type);
    virtual ~Engine();
       
    void run();
    virtual void draw() = 0;
    
    void setWindowMessage(std::string msg);

    static std::unique_ptr<Engine> create(EngineType type);

protected:

    void init_shaders();
    void draw_UiOverlay();

    ngn::MultiplatformInput input_{};
    EngineType engine_type_{};

    std::unordered_map< std::string, std::unique_ptr<Shader> > shaders_;
    std::unordered_map< std::string, std::unique_ptr<RenderObject> > fixed_objects_;
    std::vector< std::unique_ptr<RenderObject> > renderables_;
    
    Color background{};
    Camera ourCamera{};
    std::unique_ptr<Window> window_;

    size_t model_index_{0};
    const bool ui_Overlay_ = true;

private:
    void updateEvents();
    void MapActions();

    std::unordered_map<std::string, std::unique_ptr<ngn::Command>> commands_{};
    bool shouldupdate = false;

    static std::unique_ptr<Engine> makeVulkan(EngineType type);
    static std::unique_ptr<Engine> makeOpengl(EngineType type);

};

