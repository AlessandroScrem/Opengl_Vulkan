#pragma once

#include "..\Engine.hpp"
// common
#include <baseclass.hpp>
// #include <Window.hpp>


class OpenglShader;
class OpenglUbo;
class OpenglVertexBuffer;
class Window;


namespace ogl
{
class OpenGLEngine : public Engine
{    
public:
    OpenGLEngine(EngineType type);
    ~OpenGLEngine();

    void run() override;
    void setWindowMessage(std::string msg);

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

    OpenglShader & getShader(std::string name); 

    void clearBackground();
    void updateframebuffersize();
    
    // Window window{EngineType::Opengl, Engine::input_};

    const bool ui_Overlay_ = true;

};

}//namespace ogl

