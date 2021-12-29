#include "OpenGLWindow.hpp"
#include "shader.hpp"
#include "mesh.hpp"

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

OpenGLWindow::~OpenGLWindow() 
{
    std::cout << "OpenGLWindow  destructor\n";
}


void OpenGLWindow::cleanup() 
{   
    // Close OpenGL window and terminate GLFW
	glfwTerminate();
}
void OpenGLWindow::initWindow() 
{
    // Initialise GLFW
	if( !glfwInit() )
	{
        throw std::runtime_error("failed to initialize glfw!");
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    window = glfwCreateWindow( SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
    if( window == NULL ){
        std::cout << "failed to open GLFW window, they are not  OpenGL 3.3 compatible!" << std::endl;
        glfwTerminate();
        return;
    }   
}

void OpenGLWindow::initOpengl() 
{
 
    glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
}

void OpenGLWindow::mainLoop() 
{
    Shader shader{};
    Mesh mesh{};
    
    //render loop
    while(!glfwWindowShouldClose(window)) {

        // render
	    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        shader.use();
        mesh.draw();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}


void OpenGLWindow::run() 
{   
    initWindow();
    initOpengl();
    mainLoop();
    cleanup();
}

void OpenGLWindow::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
    std::cout << "button " << button << " clicked!\n";
}

