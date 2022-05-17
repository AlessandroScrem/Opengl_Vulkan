// common libs
#include "mytypes.hpp"
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
    spdlog::info("Opengl release number {} ", glGetString(GL_VERSION) );
    spdlog::info("GL_SHADING_LANGUAGE_VERSION {} ", glGetString(GL_SHADING_LANGUAGE_VERSION) );
    spdlog::info("GL_RENDERER {} ", glGetString(GL_RENDERER) );
}