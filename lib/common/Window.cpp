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
    std::cout << "Window  constructor\n";

    initWindow();
}

Window::~Window() {
    std::cout << "Window  destructor\n";

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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    if( window == NULL ){
        std::cout << "failed to open GLFW window, they are not  OpenGL 3.3 compatible!" << std::endl;
        glfwTerminate();
    }   
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window); 
}

void Window::swapBuffers() 
{ 
    glfwSwapBuffers(window); 
}