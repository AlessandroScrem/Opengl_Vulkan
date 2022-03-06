#pragma once

#include "OpenglVertexBuffer.hpp"
#include "OpenglShader.hpp"
#include "OpenglUbo.hpp"
// engine
#include <Engine.hpp>
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

    OpenglShader shader{Opengl::PHONG_SHADER};
    //OpenglShader shader{Opengl::TEXTURE_SHADER};

    OpenglUbo ubo{};
    OpenglVertexBuffer vertexBuffer{model};
};
