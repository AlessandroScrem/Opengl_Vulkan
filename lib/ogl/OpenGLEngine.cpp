#include "OpenGLEngine.hpp"

//libs
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//std

OpenGLEngine::OpenGLEngine()
{    
    SPDLOG_DEBUG("constructor"); 
    initOpenglGlobalStates(); 
}

OpenGLEngine::~OpenGLEngine() 
{
    SPDLOG_DEBUG("destructor");
    cleanup();
}

void OpenGLEngine::cleanup() 
{   
    SPDLOG_TRACE("cleanup");
}

void OpenGLEngine::initOpenglGlobalStates() 
{
    SPDLOG_TRACE("initOpenglGlobalStates");

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
    //enable vsync
    glfwSwapInterval(1);

}

void OpenGLEngine::run() 
{  
    spdlog::info("*******           START           ************");  

    while(!window.shouldClose() ) {
        glfwPollEvents();
        Engine::updateEvents();
        window.update();
        updateUbo();
        drawFrame();
        //glfwWaitEvents();
    }
    
    spdlog::info("*******           END             ************");  
}

void OpenGLEngine::drawFrame()
{
        // init frame
        clearBackground();

        // render
        ubo.bind();
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
    // rotate camera to y up
    ubo.view = glm::rotate(ourCamera.GetViewMatrix(), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(ourCamera.GetFov()), window.getWindowAspect(), 0.1f, 10.0f);
    ubo.viewPos = ourCamera.GetPosition();
}


