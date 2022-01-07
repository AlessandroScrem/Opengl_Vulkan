#pragma once

#include "mytypes.hpp"

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

    std::pair<int, int> GetWindowExtents();
    void swapBuffers();

    GLFWwindow* getWindowPtr() { return window; }


private:
    void initWindow();
    void createWindow();
    void setupCallbacks();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void window_iconify_callback(GLFWwindow* window, int iconified);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) ;

    int width{800};
    int height{600};
    std::string windowName = {"Hello Window"};
    const EngineType  engineType;

    bool is_framebufferResized = false;
    bool is_iconified = false;
    bool is_zerosize = false;

    GLFWwindow* window = nullptr;
};
