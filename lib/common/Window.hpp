#pragma once

#include "mytypes.hpp"
#include "common\Input\multiplatform_input.hpp"

// lib

//std
#include <string>


struct GLFWwindow;

class Window
{
public:
    Window(EngineType type);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    bool shouldClose();
    bool framebufferResized() {return is_framebufferResized; }
    bool waitforSize() { return is_iconified || is_zerosize ; }
    void updateframebuffersize();

    GLFWwindow* getWindowPtr() { return window; }
    std::pair<int, int> GetWindowExtents();
    float getWindowAspect() { 
        if(is_zerosize) return 1.0;
        return (float) width / height; 
    }

    void swapBuffers();

private:
    void initWindow();
    void createWindow();
    void setupCallbacks();
    void registerCallbacks();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void window_iconify_callback(GLFWwindow* window, int iconified);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) ;

    int width{800};
    int height{600};
    std::string windowName = {};
    const EngineType  engineType;
    ngn::MultiplatformInput input_ {};

    bool is_framebufferResized = false;
    bool is_iconified = false;
    bool is_zerosize = false;

    GLFWwindow* window = nullptr;
};
