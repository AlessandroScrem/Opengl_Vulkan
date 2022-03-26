#pragma once

#include "..\Engine.hpp"
#include "OpenglVertexBuffer.hpp"
#include "OpenglShader.hpp"
#include "OpenglUbo.hpp"
// common
#include <Window.hpp>

class OpenGLEngine : public Engine
{    
public:
    OpenGLEngine();
    ~OpenGLEngine();

    void run() override;
    void setWindowMessage(std::string msg) override{window.setWindowMessage(msg);};

private:
    void initOpenglGlobalStates();
    void cleanup();
    void drawFrame();
    void updateUbo();
 
    void clearBackground();
    
    Window window{EngineType::Opengl, Engine::input_};

    OpenglShader shader{Engine::glslShader};

    OpenglUbo ubo{};
    OpenglVertexBuffer vertexBuffer{Engine::model};
};
