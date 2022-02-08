#include "Window.hpp"

//std
#include <cstdlib>
#include <iostream>

//libs
#include <GL/glew.h>
//libs
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void InitOpengl(GLFWwindow* window);

Window::Window(EngineType type) : engineType{type}
{
    SPDLOG_TRACE("constructor");

    switch (type)
    {
    case EngineType::Opengl :
        windowName = "Hello Opengl";
        break;    
    case EngineType::Vulkan :
        windowName = "Hello Vulkan";
        break;    
    default:
        break;
    }


    initWindow();
    createWindow();
    setupCallbacks();
}

Window::~Window() {
    SPDLOG_TRACE("destructor");

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::initWindow() 
{
    // Initialise GLFW
	if( !glfwInit() )
	{
        throw std::runtime_error("failed to initialize glfw!");
	}

    if(engineType == EngineType::Opengl)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.5
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
        glfwWindowHint(GLFW_SAMPLES, 2); // try to get max MSAA  
    }

    if(engineType == EngineType::Vulkan)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
}

//create and open window  
void Window::createWindow() 
{
    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    if( window == NULL ){
        spdlog::critical( "failed to open GLFW window!");
        glfwTerminate();
    } 

    if(engineType == EngineType::Opengl)
    {
        InitOpengl(window);
        spdlog::info("Opengl release number {} ", glGetString(GL_VERSION) );
        spdlog::info("GL_SHADING_LANGUAGE_VERSION {} ", glGetString(GL_SHADING_LANGUAGE_VERSION) );
        spdlog::info("GL_RENDERER {} ", glGetString(GL_RENDERER) );
    }

}

void Window::setupCallbacks() 
{
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);  
    glfwSetWindowIconifyCallback(window, window_iconify_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);    
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window); 
}

void Window::updateframebuffersize() 
{
    glViewport(0, 0, width, height);
}

void Window::swapBuffers() 
{ 
    glfwSwapBuffers(window); 
}

std::pair<int, int> Window::GetWindowExtents() 
{
    // FIXME  flag will be reset here 
    // maybe implement observable pattern
    if(is_framebufferResized) {
        is_framebufferResized = false;
    }
    return { width, height };
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) 
{
    // TODO change variable name or move outside to app class
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    app->is_framebufferResized = true;
    app->width = width;
    app->height = height;
    app->is_zerosize = (!width || !height);  

    spdlog::info("window resized");   
}

void Window::window_iconify_callback(GLFWwindow* window, int iconified) 
{
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    app->is_iconified = iconified == 1 ? true : false; 
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
    spdlog::info("button {} clicked!",  button );
}
