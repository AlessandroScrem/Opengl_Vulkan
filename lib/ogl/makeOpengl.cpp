#include "OpenGLEngine.hpp"
// engine
#include <Engine.hpp>

std::unique_ptr<Engine> Engine::makeOpengl() {
    return std::make_unique<OpenGLEngine>();;
}
