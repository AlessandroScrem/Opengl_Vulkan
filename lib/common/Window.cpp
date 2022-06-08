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

Window::Window() { SPDLOG_DEBUG("constructor"); }

Window::~Window() 
{
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

    glfwSetErrorCallback( [](int error, const char * description) { spdlog::error("GLFW ERROR  {} {}", error, description );});
    
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

        #ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // require debug context  
        #endif
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
    // min area is 64 by 64
    // there is no maximum size
    glfwSetWindowSizeLimits(window_, 32, 32, GLFW_DONT_CARE, GLFW_DONT_CARE);
    is_resized = true;
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

void Window::SetWindowTitle(std::string msg) 
{
    glfwSetWindowTitle(window_, (windowName_ + msg).c_str());
}

void Window::registerCallbacks(ngn::MultiplatformInput &input) 
{
    this->Input = &input;

    SPDLOG_TRACE("registerCallbacks");  

    glfwSetWindowUserPointer(window_, &input);

    // register keyboard
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
    {
        bool io_captured = (ImGui::GetCurrentContext() == nullptr) ? false : ImGui::GetIO().WantCaptureMouse ;
        if(io_captured)
        {
            // if io captured do nothing
            return;
        } 

        // Get Multiplatform Input ptr
        auto p_input = static_cast<ngn::MultiplatformInput*>(glfwGetWindowUserPointer(window));
        if(!p_input){
            spdlog::error("Unable to get Multiplatform Input ptr ");
            return;
        }else{
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
    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) 
    {
        bool io_captured = (ImGui::GetCurrentContext() == nullptr) ? false : ImGui::GetIO().WantCaptureMouse ;        
        if(io_captured){
            // (1) ALWAYS forward mouse data to ImGui! This is automatic with default backends. With your own backend:
            ImGui::GetIO().AddMouseButtonEvent(button, action == GLFW_PRESS ? true : false);
        }else{
            // Get Multiplatform Input ptr
            auto p_input = static_cast<ngn::MultiplatformInput*>(glfwGetWindowUserPointer(window));
            if(!p_input){
                spdlog::error("Unable to get Multiplatform Input ptr ");
                return;
            }        
            // (2) ONLY forward mouse data to your underlying app/game.
            p_input->UpdateMouseState(button, action == GLFW_PRESS ? 1.f : 0.f);
        }
    });

    // // update global mouse position        
    // glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double xpos, double ypos)
    // {         
    //     // spdlog::info("mouse move.. ");
    // });


    // register window resize
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height)
    {
        // Get Multiplatform Input ptr
        auto p_input = static_cast<ngn::MultiplatformInput*>(glfwGetWindowUserPointer(window));
        if(!p_input){
            spdlog::error("Unable to get Multiplatform Input ptr ");
            return;       
        }else{
            if(width && height){
                p_input->winstat_.w = width;
                p_input->winstat_.h = height;
                p_input->winstat_.resized = true;
            }
        }
     
    });

    // register window minimize
    glfwSetWindowIconifyCallback(window_,[](GLFWwindow* window, int iconified) 
    {
        // Get Multiplatform Input ptr
        auto p_input = static_cast<ngn::MultiplatformInput*>(glfwGetWindowUserPointer(window));
        if(!p_input){
            spdlog::error("Unable to get Multiplatform Input ptr ");
            return;       
        }else{
            p_input->winstat_.iconized = iconified;
            spdlog::info("cb window iconified"); 
        }   
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
    glfwPollEvents();

    if((is_resized = Input->winstat_.resized))
    {
        width_      = Input->winstat_.w;
        height_     = Input->winstat_.h;       
        Input->winstat_.resized = false;
        assert(width_ && height_);
        spdlog::info("cb window resized {} {}",width_, height_ );    
    }

    // loop to skip iconized state
    while(Input->winstat_.iconized){
        glfwWaitEvents();    
    }
    
    {   // update global mouse position        
        double xpos, ypos;
        glfwGetCursorPos(window_, &xpos, &ypos);
        ngn::Mouse::Move((float)xpos, (float)ypos);
    }
}

GLFWwindow* Window::getWindowPtr() {
    if(!window_) {
        throw std::runtime_error("glfw Window in not yet created!");
    }
    return window_; 
}

