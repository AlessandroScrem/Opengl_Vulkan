#pragma once

#include "Engine.hpp"
#include "common/Window.hpp"
#include "common/shader.hpp"
#include "common/mesh.hpp"


class OpenGLEngine : public Engine
{    
public:
    OpenGLEngine();
    ~OpenGLEngine();

    void run() override;

private:
    void initOpenglGlobalStates();
    void mainLoop();
    void cleanup();
    void drawFrame(Shader &shader, Mesh &mesh);
 
    void clearBackground();

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    
    Window window{EngineType::Opengl};

    Shader shader{};
    Mesh mesh{};
};
