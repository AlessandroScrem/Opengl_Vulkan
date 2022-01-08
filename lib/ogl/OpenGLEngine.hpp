#pragma once

#include "Engine.hpp"
#include "common/Window.hpp"
#include "OpenglVertexBuffer.hpp"


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
    void drawFrame();
 
    void clearBackground();
    
    Window window{EngineType::Opengl};

    Shader shader{};
    OpenglVertexBuffer vertexBuffer{window};

};
