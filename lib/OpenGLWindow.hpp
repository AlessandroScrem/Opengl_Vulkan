#pragma once

#include "Window.hpp"

struct GLFWwindow;

class OpenGLWindow : public Window
{    
public:
    OpenGLWindow(){    std::cout << "OpenGLWindow  constructor\n";}
    ~OpenGLWindow();

    void run() override;

private:
    void initWindow();
    void initOpengl();
    void mainLoop();
    void cleanup();
    
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    GLFWwindow* window = nullptr;
};
