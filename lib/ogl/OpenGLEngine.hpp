#pragma once

#include "..\Engine.hpp"
#include "OpenglVertexBuffer.hpp"
#include "OpenglShader.hpp"
#include "OpenglUbo.hpp"
// common
#include <Window.hpp>
// std
#include <unordered_map>

namespace ogl{

struct RenderObject {
    
    std::unique_ptr<OpenglVertexBuffer> vertexbuffer;
    std::string  shader;
    glm::mat4 obj_trasform;
};


class OpenGLEngine : public Engine
{    
public:
    OpenGLEngine();
    ~OpenGLEngine();

    void run() override;
    void setWindowMessage(std::string msg) override{window.setWindowMessage(msg);};

private:
    void initOpenglGlobalStates();
    void init_shaders();
    void init_fixed();
    void init_renderables();
    void cleanup();
    void draw();
    void draw_fixed();
    void draw_objects();
    void updateUbo();

    void clearBackground();
    
    Window window{EngineType::Opengl, Engine::input_};
    OpenglUbo ubo{};

    std::unordered_map< std::string, std::unique_ptr<OpenglShader> > _shaders;
    std::vector<RenderObject> _renderables;
    std::unordered_map<std::string, RenderObject> _fixed_objects;
};

}// namespace ogl
