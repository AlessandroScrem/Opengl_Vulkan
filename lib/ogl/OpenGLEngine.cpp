#include "OpenglVertexBuffer.hpp"
#include "OpenglShader.hpp"
#include "OpenglUbo.hpp"
#include "OpenGLEngine.hpp"
// common lib
#include <model.hpp>
#include <Window.hpp>
//libs
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
// std

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, 
                            GLsizei length, const char *message, const void *userParam);

namespace ogl
{
OpenGLEngine::OpenGLEngine(EngineType type) : Engine(type)
{    
    SPDLOG_DEBUG("constructor"); 
    init();
}

OpenGLEngine::~OpenGLEngine() 
{
    SPDLOG_DEBUG("destructor");
    cleanup();
}

void OpenGLEngine::init()
{
    initOpenglGlobalStates();

    Shader::addBuilder(std::make_unique<OpenglShaderBuilder>());
    RenderObject::addBuilder(std::make_unique<OpenglObjectBuilder>());
    
    Engine::init_shaders(); 
    Engine::init_renderables();
    Engine::init_fixed();

    prepareUniformBuffers();

    if(ui_Overlay_){
        UIoverlay.windowPtr = window_->getWindowPtr();
        UIoverlay.init();
    }  
}

void OpenGLEngine::cleanup() 
{ 
    if (uboDataDynamic_.model) {
		delete uboDataDynamic_.model;
	} 

    if(ui_Overlay_){
        UIoverlay.cleanup();
    }  
}

void OpenGLEngine::initOpenglGlobalStates() 
{
    SPDLOG_TRACE("initOpenglGlobalStates");

    // check if we successfully initialized a debug context
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        spdlog::info("GL_DEBUG_OUTPUT ENABLED");
        // initialize debug output 
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        // test debug error
        // glBindBuffer(GL_VERTEX_ARRAY, 0);
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // using multisample
    glEnable(GL_MULTISAMPLE);

    //check opengl internals
    GLint maxSamples{};
    GLint samples{};
    GLint max_uniform_buffer_bindings{};
    GLint max_uniform_blocksize{};
    glGetIntegerv ( GL_MAX_SAMPLES, &maxSamples );
    glGetIntegerv ( GL_SAMPLES, &samples );
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffer_bindings);  
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_blocksize);  
    spdlog::info("Opengl maxSamples = {} samples = {}", maxSamples, samples);
    spdlog::info("Opengl GL_MAX_UNIFORM_BUFFER_BINDINGS = {} ", max_uniform_buffer_bindings);
    spdlog::info("Opengl GL_MAX_UNIFORM_BLOCK_SIZE = {} ", max_uniform_blocksize);

    //enable vsync glfwSwapInterval(0)
    glfwSwapInterval(0);
}

void OpenGLEngine::prepareUniformBuffers()
{
    size_t bufferSize = sizeof(glm::mat4);

    uboDataDynamic_.model = new glm::mat4;
    assert(uboDataDynamic_.model);

    openglUbo_.view = std::make_unique<OpenglUbo>(sizeof(UniformBufferObject), GLSL::ShaderBinding::UNIFORM_BUFFER, &uniformBuffer_);
    openglUbo_.dynamic = std::make_unique<OpenglUbo>(bufferSize, GLSL::ShaderBinding::UNIFORM_BUFFER_DYNAMIC, uboDataDynamic_.model);


}

void OpenGLEngine::updateUbo()
{
    UniformBufferObject mvp = Engine::getMVP();


    uniformBuffer_.model = mvp.model;
    uniformBuffer_.view  = mvp.view;
    uniformBuffer_.proj  = mvp.proj;
    uniformBuffer_.viewPos = mvp.viewPos;
    uniformBuffer_.drawLines = mvp.drawLines;


    // openglUbo_.dynamic->bind();
    
}

void OpenGLEngine::resizeFrame() 
{
    auto [w, h] = window_->extents();
    glViewport(0, 0, w, h);
}


void OpenGLEngine::draw()
{
    begin_frame();
    
        draw_fixed();
        draw_objects();

        if(ui_Overlay_){
            UIoverlay.newFrame();
            Engine::draw_UiOverlay();
            UIoverlay.draw();
        }

    end_frame();
}

void OpenGLEngine::begin_frame()
{
    // set the background color
    glClearColor( Engine::background.r,  Engine::background.g,  Engine::background.b,  Engine::background.a);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );    
}


void OpenGLEngine::end_frame()
{
    window_->swapBuffers();  
}


void OpenGLEngine::draw_fixed()
{
    // RenderObject & ro                   = *fixed_objects_.at("axis");
    // OpenglShader &shader                = dynamic_cast<OpenglShader&>(Engine::getShader(ro.shader));
    // OpenglVertexBuffer &vertexbuffer    = dynamic_cast<OpenglVertexBuffer&>(ro);

    // auto [x, y] = window_->extents();

    // // set new world origin to bottom left + offset
    // const float offset = 50; 
    // const float left   = -offset;
    // const float right  = x-offset;
    // const float bottom = -offset;
    // const float top    = y-offset;

    // UniformBufferObject mvp{};
    // mvp.view = ourCamera.GetViewMatrix();
    // mvp.proj = glm::ortho(left, right, bottom, top, -100.0f, 100.0f);
    // shader.updateUbo(mvp);

    // shader.bind(GL_LINE);
    // vertexbuffer.draw(shader.getTopology());
    
}

void OpenGLEngine::draw_objects()
{
    uint32_t index = 0;
    updateUbo();
    
    for(  auto & ro : renderables_){

        OpenglShader &shader                = dynamic_cast<OpenglShader&>(Engine::getShader(ro->shader));
        OpenglVertexBuffer &vertexbuffer    = dynamic_cast<OpenglVertexBuffer&>(*ro);

        *uboDataDynamic_.model = ro->objNode.getfinal();
        index++;

        shader.bind(GL_FILL);
        openglUbo_.view->bind();
        openglUbo_.dynamic->bind(uboDataDynamic_.model, sizeof(glm::mat4));
        vertexbuffer.draw(shader.getTopology());
    }
}


}//namespace ogl



