#include "Engine.hpp"
#include "OpenGLEngine.hpp"

std::unique_ptr<Engine> Engine::makeOpengl() {
    return std::make_unique<OpenGLEngine>();;
}
