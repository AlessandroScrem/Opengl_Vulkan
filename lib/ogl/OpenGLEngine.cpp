#include "OpenglVertexBuffer.hpp"
#include "OpenglShader.hpp"
#include "OpenglUbo.hpp"
#include "OpenGLEngine.hpp"
// common lib
#include <model.hpp>
#include <Window.hpp>
//libs
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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
    initOpenglGlobalStates();  
    init_UiOverlay();

    Shader::addBuilder(std::make_unique<OpenglShaderBuilder>());
    Engine::init_shaders(); 


    init_renderables();
    init_fixed();
}

OpenGLEngine::~OpenGLEngine() 
{
    SPDLOG_DEBUG("destructor");
    cleanup_UiOverlay();
}

void OpenGLEngine::cleanup_UiOverlay() 
{   
    SPDLOG_TRACE("cleanup_UiOverlay");
    
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
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
    glEnable(GL_CULL_FACE); 
    glEnable(GL_DEPTH_TEST);

  // using multisample
    glEnable(GL_MULTISAMPLE);

    //check opengl internals
    GLint maxSamples;
    GLint samples;
    GLint max_uniform_buffer_bindings;
    GLint max_uniform_blocksize;
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

void OpenGLEngine::init_UiOverlay()
{
	// Setup Platform/Renderer bindings
    // TODO update glsl_version acconrdigly with opengl context creation
	if(!ImGui_ImplGlfw_InitForOpenGL(window_->getWindowPtr(), true)){
        throw std::runtime_error("failed to initialize ImGui_ImplGlfw_InitForOpenGL!");
    }

    const char *glsl_version = "#version 450 core";
	if(!ImGui_ImplOpenGL3_Init(glsl_version)){
        throw std::runtime_error("failed to initialize ImGui_ImplOpenGL3_Init!");
    }
}

void OpenGLEngine::init_fixed()
{ 
    auto & model = Model::axis();
    std::unique_ptr<RenderObject> object = std::make_unique<OpenglVertexBuffer>(Model::axis());
    object->shader = "axis";
    object->model = model.get_tranform();
    fixed_objects_.emplace("axis", std::move(object));    
}

void OpenGLEngine::init_renderables()
{

    {
        Model model("data/models/viking_room.obj", Model::UP::ZUP);
        // rotate toward camera
        glm::mat4 trasf = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model.set_transform(trasf);

        std::unique_ptr<RenderObject> object = std::make_unique<OpenglVertexBuffer>(model);
        object->shader = "texture";
        object->model = model.get_tranform();
        renderables_.push_back(std::move(object));
    }

    {
        Model model("data/models/suzanne.obj", Model::UP::YUP);
        std::unique_ptr<RenderObject> object = std::make_unique<OpenglVertexBuffer>( model);
        object->shader = "normalmap";
        object->model = model.get_tranform();
        renderables_.push_back(std::move(object));

    } 
}

OpenglShader & OpenGLEngine::getShader(std::string name) 
{
    auto got = shaders_.find (name);
    if ( got == shaders_.end() ){
        throw std::runtime_error("failed to find shader!");
    }
    return static_cast<OpenglShader&>(*got->second);
} 


void OpenGLEngine::updateframebuffersize() 
{
    auto [w, h] = window_->extents();
    glViewport(0, 0, w, h);
}


void OpenGLEngine::draw()
{
    if (window_->is_Resized()){
        updateframebuffersize();
    }

    // init frame
    clearBackground();
    
    draw_fixed();
    draw_objects();

    draw_UiOverlay();

    // end frame
    window_->swapBuffers();  
}

void OpenGLEngine::draw_UiOverlay()
{
        if(!ui_Overlay_){
            return;
        }

        // feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

            Engine::draw_UiOverlay();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGLEngine::draw_fixed()
{
    RenderObject & ro                   = *fixed_objects_.at("axis");
    OpenglShader &shader                = getShader(ro.shader);
    OpenglVertexBuffer &vertexbuffer    = static_cast<OpenglVertexBuffer&>(ro);

    auto [x, y] = window_->extents();

    // set new world origin to bottom left + offset
    float offset = 50; 
    float left   = -offset;
    float right  = x-offset;
    float bottom = -offset;
    float top    = y-offset;

    UniformBufferObject mvp{};
    mvp.view = ourCamera.GetViewMatrix();
    mvp.proj = glm::ortho(left, right, bottom, top, -100.0f, 100.0f);
    shader.updateUbo(mvp);

    shader.bind();
    vertexbuffer.draw(shader.getTopology());
    
}

void OpenGLEngine::draw_objects()
{
    RenderObject & ro                   = *renderables_.at(model_index_);
    OpenglShader &shader                = getShader(ro.shader);
    OpenglVertexBuffer &vertexbuffer    = static_cast<OpenglVertexBuffer&>(ro);

   
    UniformBufferObject mvp = Engine::getMVP();        
    mvp.model = ro.model;
    shader.updateUbo(mvp);

    shader.bind();
    vertexbuffer.draw(shader.getTopology());
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

}//namespace ogl



