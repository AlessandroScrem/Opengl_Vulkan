#include "Window.hpp"
#include "service_locator.hpp"
#include "multiplatform_input.hpp"
#include "utils.hpp"
//std
#include <cstdlib>
#include <iostream>
//libs
#include <imgui.h>
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void InitOpengl(GLFWwindow* window);

Window::Window(EngineType type, ngn::MultiplatformInput &input) 
: engineType{type} 
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
    initGUI();
    registerCallbacks(input);
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

void Window::init(EngineType type)
{
    engineType = type;

    switch (engineType)
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
    initGUI();
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
    }

    is_initialized = true;
}

void Window::initGUI()
{
    // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
}

void Window::SetWindowTitle(std::string msg) {
    glfwSetWindowTitle(window_, (windowName_ + msg).c_str());
}

void Window::registerCallbacks(ngn::MultiplatformInput &input) 
{

    SPDLOG_TRACE("registerCallbacks");  

    glfwSetWindowUserPointer(window_, &input);

    // register keyboard
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        // Get the input
        auto* p_input = static_cast<ngn::MultiplatformInput*>(glfwGetWindowUserPointer(window));
        
        bool io_captured = (ImGui::GetCurrentContext() == nullptr) ? false : ImGui::GetIO().WantCaptureMouse ;
        if(!p_input || io_captured){
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

            p_input->UpdateKeyboardState(key, value);
        }
    });

    // register mouse btn
    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
        // Get the input
        auto* p_input = static_cast<ngn::MultiplatformInput*>(glfwGetWindowUserPointer(window));

        bool io_captured = (ImGui::GetCurrentContext() == nullptr) ? false : ImGui::GetIO().WantCaptureMouse ;
        if(!p_input){
            return;
        }
        
        if(io_captured){
            // (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
            ImGui::GetIO().AddMouseButtonEvent(button, action == GLFW_PRESS ? true : false);
        }else{
            // (2) ONLY forward mouse data to your underlying app/game.
            p_input->UpdateMouseState(button, action == GLFW_PRESS ? 1.f : 0.f);
        }
    });


    // register window resize
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
        
        //does nothing
        spdlog::info("cb window resized");   
    });

    // register window minimize
    glfwSetWindowIconifyCallback(window_,[](GLFWwindow* window, int iconified) {
        //does nothing
        spdlog::info("cb window iconified");
    });   
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window_); 
}


void Window::swapBuffers() 
{ 
    glfwSwapBuffers(window_); 
}


void Window::update()
{
    updateWindowSize();

    {   // update global mouse position        
        double xpos, ypos;
        glfwGetCursorPos(window_, &xpos, &ypos);
        ngn::Mouse::Move((float)xpos, (float)ypos);
    }
}

void Window::updateWindowSize() 
{
    int w, h;

    do  // loop to skip zerosized
    {   
        glfwWaitEvents();     
        glfwGetFramebufferSize(window_, &w, &h);
    }while(!w || !h);  
    

    if( (is_resized = (w != width_ || h != height_)) ) {
        width_ = w;
        height_ = h;
        spdlog::info("is  Resized = {}", is_resized);
    }       
    assert(width_ || height_);
}


GLFWwindow* Window::getWindowPtr() {
    if(!is_initialized) {
        throw std::runtime_error("glfw Window in not yet created!");
    }
    return window_; 
}

