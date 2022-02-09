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

    glfwDestroyWindow(window);
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
    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    if( window == NULL ){
        spdlog::critical( "failed to open GLFW window!");
        glfwTerminate();
    } 

    if(engineType == EngineType::Opengl)
    {
        InitOpengl(window);
        spdlog::info("Opengl release number {} ", glGetString(GL_VERSION) );
        spdlog::info("GL_SHADING_LANGUAGE_VERSION {} ", glGetString(GL_SHADING_LANGUAGE_VERSION) );
        spdlog::info("GL_RENDERER {} ", glGetString(GL_RENDERER) );
    }

}

void Window::registerCallbacks() 
{   
using namespace  ngn;

    glfwSetWindowUserPointer(window, &input_);

    // register keyboard
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
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
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        // Get the input
        auto* input = static_cast<MultiplatformInput*>(glfwGetWindowUserPointer(window));

        if (input) {
            input->UpdateMouseState(button, action == GLFW_PRESS ? 1.f : 0.f);
        }
    });

    // register mouse move
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) { 
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

    // register window resize
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

        app->is_framebufferResized = true;
        app->width = width;
        app->height = height;
        app->is_zerosize = (!width || !height);  

        spdlog::info("window resized");   
    });

    // register window minimize
    glfwSetWindowIconifyCallback(window,[](GLFWwindow* window, int iconified) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

        app->is_iconified = iconified == 1 ? true : false; ;
    });   
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window); 
}

void Window::updateframebuffersize() 
{
    glViewport(0, 0, width, height);
}

void Window::swapBuffers() 
{ 
    glfwSwapBuffers(window); 
}

std::pair<int, int> Window::GetWindowExtents() 
{
    // FIXME  flag will be reset here 
    // maybe implement observable pattern
    if(is_framebufferResized) {
        is_framebufferResized = false;
    }
    return { width, height };
}
