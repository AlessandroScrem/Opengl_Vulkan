#include "OpenGLEngine.hpp"
#include "common/shader.hpp"
#include "common/mesh.hpp"

//std
#include <cstdlib>
#include <iostream>

//libs
#include <GL/glew.h>
//libs
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


// settings
const std::string WINDOW_TITLE   = "OpenGL";
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

OpenGLEngine::~OpenGLEngine() 
{
    SPDLOG_TRACE("destructor");
    run();
}

void OpenGLEngine::run() 
{   
    initOpengl(); 
    mainLoop();
    cleanup();
}

void OpenGLEngine::cleanup() 
{   

}


void OpenGLEngine::initOpengl() 
{
 
    glfwMakeContextCurrent(window.getWindowPtr());
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window.getWindowPtr() , mouse_button_callback);

    glfwMakeContextCurrent(window.getWindowPtr() ); // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
}

void OpenGLEngine::mainLoop() 
{
    Shader shader{};
    Mesh mesh{};
    
    //render loop
    while(!window.shouldClose() ) {

        // render
	    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        shader.use();
        mesh.draw();

        // Swap buffers
        window.swapBuffers();
        glfwPollEvents();
    }
}



void OpenGLEngine::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
    spdlog::info("button {} clicked!",  button );
}

