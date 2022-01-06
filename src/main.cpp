#include "main.hpp"
#include "myApp.hpp"
#include <string>

// spdlog levels:
//
// SPDLOG_LEVEL_TRACE,
// SPDLOG_LEVEL_DEBUG,
// SPDLOG_LEVEL_INFO,
// SPDLOG_LEVEL_WARN,
// SPDLOG_LEVEL_ERROR,
// SPDLOG_LEVEL_CRITICAL,
// SPDLOG_LEVEL_OFF

EngineType parser(int argc, const char** argv)
{

    if (argc == 1)
    {   
        // default
        return EngineType::Opengl;
    }

    std::string arg1(argv[1]);
    if (arg1 == "--vulkan"){
        return EngineType::Vulkan;
    }
    if (arg1 == "--opengl"){
        return EngineType::Opengl;
    }
}

int main(int argc, char const **argv)
{
    EngineType engine = parser(argc, argv);

    spdlog::set_level(spdlog::level::trace);

    try {
        auto app = Engine::create(engine);
        app->run();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
