#include "Window.hpp"
#include "VulkanWindow.hpp"
#include "OpenGLWindow.hpp"


std::unique_ptr<Window>
Window::create(WindowType type)
{
    std::unique_ptr<Window> window;

    if(type == WindowType::Opengl){
        window = std::make_unique<OpenGLWindow>();
    }

    if(type == WindowType::Vulkan){
        window = std::make_unique<VulkanWindow>();
    } 

    return window;   
}
