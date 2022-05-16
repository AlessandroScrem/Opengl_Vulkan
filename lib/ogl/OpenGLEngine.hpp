#pragma once

#include "..\Engine.hpp"
#include "OpenglVertexBuffer.hpp"
#include "OpenglShader.hpp"
#include "OpenglUbo.hpp"
// common
#include <baseclass.hpp>
#include <Window.hpp>
// std
#include <unordered_map>
//libs

namespace ogl
{

class OpenGLEngine : public Engine
{    
public:
    OpenGLEngine();
    ~OpenGLEngine();

    void run() override;
    void setWindowMessage(std::string msg) override{window.setWindowMessage(msg);};

private:
    void initOpenglGlobalStates();
    void initGUI();
    void init_shaders();
    void init_fixed();
    void init_renderables();
    void cleanup();
    void draw();
    void draw_overlay();
    void draw_fixed();
    void draw_objects();
    void updateUbo(OpenglUbo &ubo);

    OpenglShader & getShader(std::string name) {
        auto got = _shaders.find (name);
        if ( got == _shaders.end() ){
            throw std::runtime_error("failed to find shader!");
        }
        return static_cast<OpenglShader&>(*got->second);
    }   

    void clearBackground();
    
    Window window{EngineType::Opengl, Engine::input_};

    const bool _overlay = true;

};

}//namespace ogl

