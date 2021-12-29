#include "main.hpp"
#include "myApp.hpp"

int main(int argc, char const *argv[])
{
    try {
        //auto app = Window::create(WindowType::Opengl);
        auto app = Window::create(WindowType::Vulkan);
        app->run();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
