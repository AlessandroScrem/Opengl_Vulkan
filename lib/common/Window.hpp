#pragma once

#include "mytypes.hpp"
// #include "multiplatform_input.hpp"

// lib

//std
#include <string>


struct GLFWwindow;
namespace ngn
{
    class MultiplatformInput;
}

class Window
{
public:
    Window(EngineType type, ngn::MultiplatformInput &input);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    bool shouldClose();
    bool is_Resized() {return is_resized;}

    void update();
    void setWindowMessage(std::string msg){SetWindowTitle(msg);}

    GLFWwindow* getWindowPtr();
    float getWindowAspect() { return (float) width_ / height_; }
    std::pair<uint32_t, uint32_t> extents() { return {width_ ,height_}; }


    void swapBuffers();

private:
    void initWindow();
    void createWindow();
    void updateWindowSize();
    void initGUI();
    void registerCallbacks(ngn::MultiplatformInput &input);
    void SetWindowTitle(std::string msg = "");


    int width_{800};
    int height_{600};
    std::string windowName_ = {};
    const EngineType  engineType;

    bool is_resized = false;
    bool is_initialized = false;

    GLFWwindow* window_ = nullptr;
};
