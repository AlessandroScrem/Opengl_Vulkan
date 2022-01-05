#include "Window.hpp"

//std
#include <cstdlib>
#include <iostream>

//libs
#include <GL/glew.h>
//libs
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


Window::Window(EngineType type) : engineType{type}
{
    SPDLOG_TRACE("constructor");

    initWindow();
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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
    }

    if(engineType == EngineType::Vulkan)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    if( window == NULL ){
        spdlog::critical( "failed to open GLFW window, they are not  OpenGL 3.3 compatible!");
        glfwTerminate();
    } 

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);  
    glfwSetWindowIconifyCallback(window, window_iconify_callback);
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window); 
}

void Window::swapBuffers() 
{ 
    glfwSwapBuffers(window); 
}

std::pair<int, int> Window::GetWindowExtents() 
{
    // FIXME  flag will be reset here 
    if(is_framebufferResized) {is_framebufferResized = false;}
    return { width, height };
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) 
{
    // TODO change variable name or move outside to app class
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    app->is_framebufferResized = true;
    app->width = width;
    app->height = height;

    app->is_zerosize = false;
    if(width == 0 || height == 0){
        app->is_zerosize = true;
    }

    spdlog::info("window resized");   
}

void Window::window_iconify_callback(GLFWwindow* window, int iconified) 
{
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    app->is_iconified = iconified == 1 ? true : false; 
}