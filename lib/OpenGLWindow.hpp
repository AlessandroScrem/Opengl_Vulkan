#pragma once

struct GLFWwindow;

class OpenGLWindow
{    
public:
    OpenGLWindow(){}
    ~OpenGLWindow();

    void setup();
    void run();

private:
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    GLFWwindow* window = nullptr;
};
