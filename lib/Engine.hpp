#pragma once

#include "mytypes.hpp"
#include "common\camera.hpp"
#include "common/model.hpp"

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
    Engine(){ SPDLOG_TRACE("constructor");     
    }
    virtual ~Engine(){SPDLOG_TRACE("destructor");}
       
    virtual void run() = 0;

   static std::unique_ptr<Engine> create(EngineType type);

protected:
    Model model{};
    Color background{};
    Camera ourCamera{glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}; 

private:
    static std::unique_ptr<Engine> makeVulkan();
    static std::unique_ptr<Engine> makeOpengl();
};

