#pragma once

#include "Engine.hpp"
#include "common/Window.hpp"
#include "OpenglVertexBuffer.hpp"
#include "OpenglShader.hpp"
#include "OpenglUbo.hpp"




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
    void updateUbo();
 
    void clearBackground();
    
    Window window{EngineType::Opengl, Engine::input_};

    OpenglShader shader{};
    OpenglUbo ubo{};
    OpenglVertexBuffer vertexBuffer{window, model};

};
