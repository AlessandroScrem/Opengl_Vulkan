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

namespace ogl
{
OpenGLEngine::OpenGLEngine(EngineType type) : Engine(type)
{    
    SPDLOG_DEBUG("constructor"); 
    initOpenglGlobalStates();  
    initGUI();

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
    
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void OpenGLEngine::setWindowMessage(std::string msg) 
{
    window_->setWindowMessage(msg);
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

void OpenGLEngine::initGUI()
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

void OpenGLEngine::init_shaders()
{
    SPDLOG_TRACE("init_shaders");
    {
        auto shader = std::make_unique<OpenglShader>(GLSL::TEXTURE);
        shader->addUbo(0);
        shader->addTexture("data/textures/viking_room.png", 1);
        shader->buid();

        shaders_.emplace("texture", std::move(shader));
    }
    {
        auto shader = std::make_unique<OpenglShader>(GLSL::NORMALMAP);
        shader->addUbo(0);
        shader->buid();

        shaders_.emplace("normalmap", std::move(shader));
    }
    {
        auto shader = std::make_unique<OpenglShader>(GLSL::AXIS);
        shader->addUbo(0);
        shader->setPolygonMode(GL_LINE);
        shader->setTopology(GL_LINES);
        shader->buid();

        shaders_.emplace("axis", std::move(shader));
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

void OpenGLEngine::run() 
{  
    spdlog::info("*******           START           ************");  

    while(!window_->shouldClose() ) {
        glfwPollEvents();
        Engine::updateEvents();
        window_->update();
        draw();
    }
    
    spdlog::info("*******           END             ************");  
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

    draw_overlay();

    // end frame
    window_->swapBuffers();  
}

void OpenGLEngine::draw_overlay()
{
        if(!ui_Overlay_){
            return;
        }

        // feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
 
        // render your GUI
		ImGui::Begin("Triangle Position/Color");
		static float rotation = 0.0;
		ImGui::SliderFloat("rotation", &rotation, 0, 2 * 3.14f);
		static float translation[] = {0.0, 0.0};
		ImGui::SliderFloat2("position", translation, -1.0, 1.0);
        static float color[4] = { 1.0f,1.0f,1.0f,1.0f };
         // color picker
        ImGui::ColorEdit3("color", color);
        ImGui::End();
        
        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGLEngine::draw_fixed()
{
    RenderObject & ro                   = *fixed_objects_.at("axis");
    OpenglShader &shader                = getShader(ro.shader);
    OpenglUbo & ubo                     = shader.getUbo();
    OpenglVertexBuffer &vertexbuffer    = static_cast<OpenglVertexBuffer&>(ro);

    auto [x, y] = window_->extents();

    // set new world origin to bottom left + offset
    float offset = 50; 
    float left   = -offset;
    float right  = x-offset;
    float bottom = -offset;
    float top    = y-offset;

    ubo.view = ourCamera.GetViewMatrix();
    ubo.proj = glm::ortho( left , right , bottom , top, -1000.0f, 1000.0f);
    ubo.bind();

    shader.use();
    vertexbuffer.draw(shader.getTopology());
    
}

void OpenGLEngine::draw_objects()
{
    RenderObject & ro                   = *renderables_.at(model_index_);
    OpenglShader &shader                = getShader(ro.shader);
    OpenglUbo & ubo                     = shader.getUbo();
    OpenglVertexBuffer &vertexbuffer    = static_cast<OpenglVertexBuffer&>(ro);

    updateUbo(ubo);
    ubo.model = ro.model;

    // render
    updateUbo(ubo);
    ubo.bind();

    shader.use();
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

void OpenGLEngine::updateUbo(OpenglUbo &ubo)
{
    ubo.view = ourCamera.GetViewMatrix();
    ubo.proj = glm::perspective(glm::radians(ourCamera.GetFov()), window_->getWindowAspect(), 0.1f, 10.0f);
    ubo.viewPos = ourCamera.GetPosition();
}

}//namespace ogl



