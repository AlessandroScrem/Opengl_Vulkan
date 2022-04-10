#include "OpenGLEngine.hpp"

//libs
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//std
namespace ogl
{

OpenGLEngine::OpenGLEngine()
{    
    SPDLOG_DEBUG("constructor"); 
    initOpenglGlobalStates(); 

    init_renderables();

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

void OpenGLEngine::init_renderables()
{
    _shaders.emplace_back(std::make_unique<OpenglShader>(GLSL::PHONG_SHADER) );  
    _shaders.emplace_back(std::make_unique<OpenglShader>(GLSL::NORMALMAP_SHADER) );  
    auto  &shader = *_shaders.at(1);

    for(auto & mod : _models)
    {
        std::unique_ptr<OpenglVertexBuffer> vb = std::make_unique<OpenglVertexBuffer>(mod);
    
        _renderables.push_back(RenderObject{
            std::move(vb),
            shader, 
            mod.get_tranform() }
        ); 
    }   
}

void OpenGLEngine::run() 
{  
    spdlog::info("*******           START           ************");  

    while(!window.shouldClose() ) {
        glfwPollEvents();
        Engine::updateEvents();
        window.update();
        updateUbo();
        draw();
        //glfwWaitEvents();
    }
    
    spdlog::info("*******           END             ************");  
}

void OpenGLEngine::draw()
{
        // init frame
        clearBackground();
        draw_objects();
 
        // end frame
        window.updateframebuffersize();
        window.swapBuffers();  
}

void OpenGLEngine::draw_objects()
{
        RenderObject & ro = _renderables.at(_model_index);
        OpenglVertexBuffer &vertexBuffer = *ro.vertexbuffer;
        // render
        ubo.model = ro.obj_trasform;      
        ubo.bind();

        ro.shader.use();
        vertexBuffer.draw();
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

    ubo.view = ourCamera.GetViewMatrix();
    ubo.proj = glm::perspective(glm::radians(ourCamera.GetFov()), window.getWindowAspect(), 0.1f, 10.0f);
    ubo.viewPos = ourCamera.GetPosition();
}

}//namespace ogl



