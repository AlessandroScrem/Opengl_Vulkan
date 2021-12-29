#include "main.hpp"
#include "myApp.hpp"

int main(int argc, char const *argv[])
{
    try {
        //auto app = Engine::create(EngineType::Opengl);
        auto app = Engine::create(EngineType::Vulkan);
        app->run();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
