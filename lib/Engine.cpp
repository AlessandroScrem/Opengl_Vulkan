#include "Engine.hpp"
#include "VulkanEngine.hpp"
#include "OpenGLEngine.hpp"


std::unique_ptr<Engine>
Engine::create(EngineType type)
{
    std::unique_ptr<Engine> window;

    if(type == EngineType::Opengl){
        window = std::make_unique<OpenGLEngine>();
    }

    if(type == EngineType::Vulkan){
        window = std::make_unique<VulkanEngine>();
    } 

    return window;   
}
