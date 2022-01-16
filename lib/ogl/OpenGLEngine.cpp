#include "OpenGLEngine.hpp"

//libs
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//std

OpenGLEngine::OpenGLEngine()
{    
    SPDLOG_TRACE("constructor"); 
}

OpenGLEngine::~OpenGLEngine() 
{
    SPDLOG_TRACE("destructor");

}

void OpenGLEngine::run() 
{   
    initOpenglGlobalStates(); 
    mainLoop();
    cleanup();
}

void OpenGLEngine::cleanup() 
{   

}

void OpenGLEngine::initOpenglGlobalStates() 
{
    // configure global opengl state
    // -----------------------------
    //glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glEnable(GL_CULL_FACE); 
    glCullFace(GL_BACK); 
    //glFrontFace(GL_CW); // revert winding order as Vulkan default
    glFrontFace(GL_CCW ); // default 
    
    glEnable(GL_DEPTH_TEST);
}

void OpenGLEngine::mainLoop() 
{
    // Shader shader{};
    // Mesh mesh{};

    while(!window.shouldClose() ) {
        glfwPollEvents();
        drawFrame();
    }
}
void OpenGLEngine::drawFrame()
{
        clearBackground();

        vertexBuffer.updateUniformBuffers();
        vertexBuffer.bindUniformBuffers();

        // render
        shader.use();
        vertexBuffer.draw();

        // Swap buffers
        window.updateframebuffersize();
        window.swapBuffers();  
}

void OpenGLEngine::clearBackground()
{
        // set the background color
        float r = Engine::background.red;
        float g = Engine::background.green;
        float b = Engine::background.blue;
        float a = Engine::background.alpha;
	    glClearColor(r, g, b, a);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );    
}


