#include "OpenGLEngine.hpp"


//std
#include <cstdlib>
#include <iostream>

//libs
#include <GL/glew.h>
//libs
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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
        shader.use();

       // retrieve the matrix uniform locations
        auto ubo = vertexBuffer.getUbo();
        shader.setMat4("ubo.proj", ubo.proj);
        shader.setMat4("ubo.view", ubo.view);
        shader.setMat4("ubo.model", ubo.model);

        

        // render
        //mesh.draw();
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


