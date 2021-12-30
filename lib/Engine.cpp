#include "Engine.hpp"

std::unique_ptr<Engine>
Engine::create(EngineType type)
{
    std::unique_ptr<Engine> window;

    if(type == EngineType::Opengl){
        //window = std::make_unique<OpenGLEngine>();
        window = makeOpengl();
    }

    if(type == EngineType::Vulkan){
        //window = std::make_unique<VulkanEngine>();
        window = makeVulkan();
    }

    return window;   
}

