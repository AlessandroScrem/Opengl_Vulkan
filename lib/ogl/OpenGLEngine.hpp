#pragma once

#include "..\Engine.hpp"
#include "OpenglUIOverlay.h"
// common
#include <baseclass.hpp>


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

protected:
    void draw() override;
    void resizeFrame() override;
private:

    void initOpenglGlobalStates();
    void prepareUniformBuffers();
    void updateUbo();
    void init();
    void begin_frame();
    void draw_fixed();
    void draw_objects();
    void end_frame();
    void cleanup();

    OpenglUIOverlay UIoverlay{};

    struct {
        std::unique_ptr<OpenglUbo> view;
        std::unique_ptr<OpenglUbo> dynamic;
        size_t dynamicAlignment;
    }openglUbo_;

};
}//namespace ogl

