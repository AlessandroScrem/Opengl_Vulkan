#include "Window.hpp"
#include "common\Input\service_locator.hpp"
#include "common\input\utils.hpp"

//std
#include <cstdlib>
#include <iostream>

//libs
#include <GL/glew.h>
//libs
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void InitOpengl(GLFWwindow* window);

Window::Window(EngineType type) : engineType{type}
{
    SPDLOG_TRACE("constructor");

    switch (type)
    {
    case EngineType::Opengl :
        windowName = "Hello Opengl";
        break;    
    case EngineType::Vulkan :
        windowName = "Hello Vulkan";
        break;    
    default:
        break;
    }

    // provide input manager
    ngn::ServiceLocator::Provide(new ngn::InputManager());

    initWindow();
    createWindow();
    registerCallbacks();
}

Window::~Window() {
    SPDLOG_TRACE("destructor");

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
    window_ = glfwCreateWindow(width_, height_, windowName.c_str(), nullptr, nullptr);
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

}

void Window::registerCallbacks() 
{   
using namespace  ngn;

    glfwSetWindowUserPointer(window_, &input_);

    // register keyboard
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        // Get the input
        auto* input = static_cast<MultiplatformInput*>(glfwGetWindowUserPointer(window));

        if (input) {
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

        if (input) {
            input->UpdateMouseState(button, action == GLFW_PRESS ? 1.f : 0.f);
        }
    });

    // register mouse move
    glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double xpos, double ypos) { 
        // Get the mouse pos
        Mouse::Move((float)xpos, (float)ypos);           
    }); 

    // Register input devices
    auto* inputManager = ServiceLocator::GetInputManager();

    inputManager->RegisterDevice(InputDevice {
        .Type = InputDeviceType::KEYBOARD,
        .Index = 0,
        .StateFunc = std::bind(&MultiplatformInput::GetKeyboardState, &input_, std::placeholders::_1)
    });

    inputManager->RegisterDevice(InputDevice {
        .Type = InputDeviceType::MOUSE,
        .Index = 0,
        .StateFunc = std::bind(&MultiplatformInput::GetMouseState, &input_, std::placeholders::_1)
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
    GetWindowExtents();
}

std::pair<int, int> Window::GetWindowExtents() 
{
    int w, h;
    glfwGetFramebufferSize(window_, &w, &h);
    if(is_zerosize = (!w || !h) ) {
        spdlog::info("is_framebufferResized = {}", is_zerosize);
    }
    if(is_framebufferResized = (w != width_ || h != height_) ) {
        width_ = w;
        height_ = h;
        spdlog::info("is_framebufferResized = {}", is_framebufferResized);

    }
    return { width_, width_ };
}

