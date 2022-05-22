#include "main.hpp"
#include <string>


EngineType parser(int argc, const char** argv)
{
    EngineType eng_type{EngineType::Opengl};
    
    #ifdef VULKAN
        eng_type = EngineType::Vulkan;
    #endif// VULKAN 

    #ifdef   OPENGL
        eng_type = EngineType::Opengl;
    #endif// OPENGL

    if (argc == 2 && std::string{argv[1]} == "--vulkan"){
        eng_type = EngineType::Vulkan;
    }
    if (argc == 2 && std::string{argv[1]} == "--opengl"){
        eng_type = EngineType::Opengl;
    }

    return eng_type;
}

int main(int argc, char const **argv)
{
    #ifdef _DEBUG
        spdlog::set_level(spdlog::level::debug);
    #else 
        spdlog::set_level(spdlog::level::info);
    #endif
    
    EngineType eng_type = parser(argc, argv);
    
    try {
        auto app = Engine::create(eng_type);
        app->run();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    spdlog::info("EXIT_SUCCESS");
    spdlog::shutdown();

    return EXIT_SUCCESS;
}
