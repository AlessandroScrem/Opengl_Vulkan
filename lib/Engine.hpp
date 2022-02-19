#pragma once

#include "mytypes.hpp"
#include "common/camera.hpp"
#include "common/model.hpp"
#include "ngn_command.hpp"
#include "common/Input/multiplatform_input.hpp"

//std
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
    virtual ~Engine(){SPDLOG_TRACE("destructor");}
       
    virtual void run() = 0;


   static std::unique_ptr<Engine> create(EngineType type);

protected:
    ngn::MultiplatformInput input_{};
    void updateEvents();
    
    Model model{};
    Color background{};
    Camera ourCamera{glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}; 

private:
    std::unordered_map<std::string, std::unique_ptr<ngn::Command>> commands_{};
    void MapActions();
    bool shouldupdate = false;


    static std::unique_ptr<Engine> makeVulkan();
    static std::unique_ptr<Engine> makeOpengl();
};

