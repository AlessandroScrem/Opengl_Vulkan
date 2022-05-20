#pragma once
#include "mytypes.hpp"
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
    Window();
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    bool shouldClose();
    bool is_Resized() { return is_resized; }

    void init(EngineType type);
    void update();
    void registerCallbacks(ngn::MultiplatformInput &input);
    void swapBuffers();
    void setWindowMessage(std::string msg) { SetWindowTitle(msg); }

    GLFWwindow* getWindowPtr();
    float getWindowAspect() { return (float) width_ / height_; }
    std::pair<uint32_t, uint32_t> extents() { return {width_ ,height_}; }



private:
    void initWindow();
    void createWindow();
    void initGUI();
    void SetWindowTitle(std::string msg = "");


    int width_{800};
    int height_{600};
    std::string windowName_ = {};
    EngineType  engineType;
    ngn::MultiplatformInput *Input;

    bool is_resized = false;
    bool is_initialized = false;

    GLFWwindow* window_ = nullptr;
};
