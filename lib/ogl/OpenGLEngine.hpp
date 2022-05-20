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

    void draw() override;

private:
    void initOpenglGlobalStates();
    void init_UiOverlay();
    void cleanup_UiOverlay();
    void init_fixed();
    void init_renderables();
    void draw_UiOverlay();
    void draw_fixed();
    void draw_objects();
    void updateUbo(OpenglUbo &ubo);

    OpenglShader & getShader(std::string name); 

    void clearBackground();
    void updateframebuffersize();

};
}//namespace ogl

