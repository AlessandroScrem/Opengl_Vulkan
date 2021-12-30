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
    void swapBuffers();
    GLFWwindow* getWindowPtr() { return window; }

private:
    void initWindow();
    
    const int width{800};
    const int height{600};
    std::string windowName = {"Hello Window"};
    const EngineType  engineType;

    GLFWwindow* window = nullptr;
};
