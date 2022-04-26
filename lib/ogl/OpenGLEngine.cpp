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

    init_shaders();
    init_renderables();
    init_fixed();

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
    glEnable(GL_CULL_FACE); 
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
void OpenGLEngine::init_shaders()
{
    _shaders.emplace("phong", std::make_unique<OpenglShader>(GLSL::PHONG) );  
    _shaders.emplace("normalmap", std::make_unique<OpenglShader>(GLSL::NORMALMAP) );  
    _shaders.emplace("axis", std::make_unique<OpenglShader>(GLSL::AXIS) );  
}

void OpenGLEngine::init_fixed()
{ 
    _fixed_objects.emplace("axis", 
        RenderObject{
            std::make_unique<OpenglVertexBuffer>(Model::axis()),
            "axis", 
            Model::axis().get_tranform() 
        }
    );  
}

void OpenGLEngine::init_renderables()
{

    for(auto & mod : _models)
    {   
        _renderables.push_back(RenderObject{
            std::move(std::make_unique<OpenglVertexBuffer>(mod)),
            "normalmap", 
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
        draw_fixed();
        draw_objects();
 
        // end frame
        window.updateframebuffersize();
        window.swapBuffers();  
}

void OpenGLEngine::draw_fixed()
{
    RenderObject & ro = _fixed_objects.at("axis");
    OpenglVertexBuffer & vb = *ro.vertexbuffer;
    OpenglShader & shader = *_shaders.at(ro.shader);

    int x, y;
    window.extents(x, y);

    // set new world origin to bottom left + offset
    float offset = 50; 
    float left   = -offset;
    float right  = x-offset;
    float bottom = -offset;
    float top    = y-offset;
 
    ubo.proj = glm::ortho( left , right , bottom , top, -1000.0f, 1000.0f);

    ubo.bind();

    shader.use();
    vb.draw(GL_LINES);

    updateUbo();
    
}

void OpenGLEngine::draw_objects()
{
        RenderObject & ro = _renderables.at(_model_index);
        OpenglVertexBuffer &vertexBuffer = *ro.vertexbuffer;
        OpenglShader & shader = *_shaders.at(ro.shader);
        // render
        ubo.model = ro.obj_trasform;      
        ubo.bind();

        shader.use();
        vertexBuffer.draw(GL_TRIANGLES);
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



