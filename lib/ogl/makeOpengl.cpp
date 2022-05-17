#include "OpenGLEngine.hpp"

std::unique_ptr<Engine> Engine::makeOpengl(EngineType type) {
    return std::make_unique<ogl::OpenGLEngine>(type);
}

