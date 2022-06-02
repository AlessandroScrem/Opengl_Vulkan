#pragma once

#include "ngn_command.hpp"
//common lib
#include <baseclass.hpp>
#include <mytypes.hpp>
#include <camera.hpp>
#include <vertex.h>
#include <multiplatform_input.hpp>
//std
#include <vector>
#include <memory>


class Window;

class Engine
{    
public:
    Engine(EngineType type);
    virtual ~Engine();
       
    static std::unique_ptr<Engine> create(EngineType type);
    void run();

protected:

    void init_shaders();
    void init_fixed();
    void init_renderables();
    void draw_UiOverlay();
 
    /**
     * @brief Get the Shader object from shaders collection
     * 
     * @param name shader name
     * @return VulkanShader& 
     */
    Shader & getShader(std::string name) 
    {
        auto got = shaders_.find (name);
        if ( got == shaders_.end() ){
            throw std::runtime_error("failed to find shader!");
        }
        return (*got->second);
    }  

    UniformBufferObject getMVP();

    ngn::MultiplatformInput input_{};
    EngineType engine_type_{};

    std::unordered_map< std::string, std::unique_ptr<Shader> > shaders_;
    std::unordered_map< std::string, std::unique_ptr<RenderObject> > fixed_objects_;
    std::vector< std::unique_ptr<RenderObject> > renderables_;
    
    glm::vec4 background{0.2f, 0.3f, 0.3f, 1.0f};
    Camera ourCamera{};
    std::unique_ptr<Window> window_;

    size_t model_index_{0};
    const bool ui_Overlay_ = true;

private:

    virtual void draw() = 0;
    virtual void resizeFrame() = 0;

    void updateEvents();
    void MapActions();
    void setWindowMessage(std::string msg);

    std::unordered_map<std::string, std::unique_ptr<ngn::Command>> commands_{};
    bool shouldupdate = false;

    static std::unique_ptr<Engine> makeVulkan(EngineType type);
    static std::unique_ptr<Engine> makeOpengl(EngineType type);

};

