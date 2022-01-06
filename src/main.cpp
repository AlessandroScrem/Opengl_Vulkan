#include "main.hpp"
#include "myApp.hpp"

// spdlog levels:
//
// SPDLOG_LEVEL_TRACE,
// SPDLOG_LEVEL_DEBUG,
// SPDLOG_LEVEL_INFO,
// SPDLOG_LEVEL_WARN,
// SPDLOG_LEVEL_ERROR,
// SPDLOG_LEVEL_CRITICAL,
// SPDLOG_LEVEL_OFF

int main(int argc, char const *argv[])
{
    spdlog::set_level(spdlog::level::trace);

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
