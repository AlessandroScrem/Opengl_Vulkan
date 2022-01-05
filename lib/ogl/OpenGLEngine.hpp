#pragma once

#include "Engine.hpp"
#include "common/Window.hpp"

class OpenGLEngine : public Engine
{    
public:
    OpenGLEngine(){    SPDLOG_TRACE("constructor"); }
    ~OpenGLEngine();

    void run() override;

private:
    void initOpengl();
    void mainLoop();
    void cleanup();
    
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    Window window{EngineType::Opengl};
};
