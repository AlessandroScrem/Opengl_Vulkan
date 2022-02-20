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

  // using multisample
    glEnable(GL_MULTISAMPLE);
    GLint maxSamples;
    GLint samples;
    glGetIntegerv ( GL_MAX_SAMPLES, &maxSamples );
    glGetIntegerv ( GL_SAMPLES, &samples );
    spdlog::info("Opengl maxSamples = {}", maxSamples);
    spdlog::info("Opengl samples = {}", samples);

}

void OpenGLEngine::mainLoop() 
{
    while(!window.shouldClose() ) {
        Engine::updateEvents();
        window.update();
        drawFrame();
        glfwWaitEvents();
    }
}
void OpenGLEngine::drawFrame()
{
        // init frame
        clearBackground();

        // render
        updateUbo();
        shader.use();
        vertexBuffer.draw();

        // end frame
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

void OpenGLEngine::updateUbo()
{
    // rotate model to y up
    ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ubo.view = ourCamera.GetViewMatrix();
    ubo.proj = glm::perspective(glm::radians(ourCamera.GetFov()), window.getWindowAspect(), 0.1f, 10.0f);

    // update uniform buffer data
    ubo.bind();
}


