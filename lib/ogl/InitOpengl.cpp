//libs
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//std
#include <iostream>

void InitOpengl(GLFWwindow* window) 
{
    glfwMakeContextCurrent(window ); // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
}