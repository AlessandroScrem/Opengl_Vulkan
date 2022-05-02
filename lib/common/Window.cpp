#include "Window.hpp"
#include "service_locator.hpp"
#include "utils.hpp"

//std
#include <cstdlib>
#include <iostream>

//libs
#include <imgui.h>
#include <GL/glew.h>
//libs
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void InitOpengl(GLFWwindow* window);

Window::Window(EngineType type, ngn::MultiplatformInput &input) 
: engineType{type} 
, input_{input}
{
    SPDLOG_DEBUG("constructor");

    switch (type)
    {
    case EngineType::Opengl :
        windowName_ = "Hello Opengl";
        break;    
    case EngineType::Vulkan :
        windowName_ = "Hello Vulkan";
        break;    
    default:
        break;
    }

    initWindow();
    createWindow();
    registerCallbacks();
}

Window::~Window() {
    SPDLOG_DEBUG("destructor");
    
    // Cleanup ImGui
    if (ImGui::GetCurrentContext()) {
        ImGui::DestroyContext();
    } 

    glfwDestroyWindow(window_);
    glfwTerminate();
}

void Window::initWindow() 
{
    // Initialise GLFW
	if( !glfwInit() )
	{
        throw std::runtime_error("failed to initialize glfw!");
	}

    if(engineType == EngineType::Opengl)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.5
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
        glfwWindowHint(GLFW_SAMPLES, 2); // try to get max MSAA  
    }

    if(engineType == EngineType::Vulkan)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
}

//create and open window  
void Window::createWindow() 
{
    window_ = glfwCreateWindow(width_, height_, windowName_.c_str(), nullptr, nullptr);
    if( window_ == NULL ){
        spdlog::critical( "failed to open GLFW window!");
        glfwTerminate();
    } 

    if(engineType == EngineType::Opengl)
    {
        InitOpengl(window_);
        spdlog::info("Opengl release number {} ", glGetString(GL_VERSION) );
        spdlog::info("GL_SHADING_LANGUAGE_VERSION {} ", glGetString(GL_SHADING_LANGUAGE_VERSION) );
        spdlog::info("GL_RENDERER {} ", glGetString(GL_RENDERER) );
    }

    // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

    is_initialized = true;

}

void Window::SetWindowTitle(std::string msg) {
    glfwSetWindowTitle(window_, (windowName_ + msg).c_str());
}

void Window::registerCallbacks() 
{

    SPDLOG_TRACE("registerCallbacks");  
    using namespace  ngn;

    glfwSetWindowUserPointer(window_, &input_);

    // register keyboard
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        // Get the input
        auto* input = static_cast<MultiplatformInput*>(glfwGetWindowUserPointer(window));

        
        bool io_captured = (ImGui::GetCurrentContext() == nullptr) ? false : ImGui::GetIO().WantCaptureMouse ;
        if(!input || io_captured){
            return;
        }else{

            // ONLY forward keys data to your underlying app/game. 
            // set the new value for key
            float value = 0.f;

            switch (action) {
                case GLFW_PRESS:
                case GLFW_REPEAT:
                    value = 1.f;
                    break;
                default:
                    value = 0.f;
            }

            input->UpdateKeyboardState(key, value);
        }
    });

    // register mouse btn
    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
        // Get the input
        auto* input = static_cast<MultiplatformInput*>(glfwGetWindowUserPointer(window));

        bool io_captured = (ImGui::GetCurrentContext() == nullptr) ? false : ImGui::GetIO().WantCaptureMouse ;
        if(!input){
            return;
        }
        
        if(io_captured){
            // (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
            ImGui::GetIO().AddMouseButtonEvent(button, action == GLFW_PRESS ? true : false);
        }else{
            // (2) ONLY forward mouse data to your underlying app/game.
            input->UpdateMouseState(button, action == GLFW_PRESS ? 1.f : 0.f);
        }
    });

    // register window resize
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) { 
        //does nothing
        spdlog::info("window resized");   
    });

    // register window minimize
    glfwSetWindowIconifyCallback(window_,[](GLFWwindow* window, int iconified) {
        //does nothing
        spdlog::info("window iconified");
    });   
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window_); 
}

void Window::updateframebuffersize() 
{
    if (is_framebufferResized){
        glViewport(0, 0, width_, height_);
    }
}

void Window::swapBuffers() 
{ 
    glfwSwapBuffers(window_); 
}


void Window::update(){
 
    double xpos, ypos;
    glfwGetCursorPos(window_, &xpos, &ypos);
    // update global mouse position 
    ngn::Mouse::Move((float)xpos, (float)ypos);

    GetWindowExtents();
}

std::pair<int, int> Window::GetWindowExtents() 
{
    int w, h;
    glfwGetFramebufferSize(window_, &w, &h);
    if( (is_zerosize = (!w || !h)) ) {
        spdlog::info("is_zerosize = {}", is_zerosize);
    }
    if( (is_framebufferResized = (w != width_ || h != height_)) ) {
        width_ = w;
        height_ = h;
        spdlog::info("is_framebufferResized = {}", is_framebufferResized);

    }
    return { width_, height_ };
}

GLFWwindow* Window::getWindowPtr() {
    if(!is_initialized) {
        throw std::runtime_error("glfw Window in not yet created!");
    }
    return window_; 
}

