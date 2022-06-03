#pragma once
#include "mytypes.hpp"
// lib
//std
#include <string>

const int WIDTH = 800;
const int HEIGTH = 600;

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
    inline bool is_Resized() { return is_resized; }

    void init(EngineType type);
    void update();
    void swapBuffers();
    void registerCallbacks(ngn::MultiplatformInput &input);
    inline void setWindowMessage(std::string msg) { SetWindowTitle(msg); }

    GLFWwindow* getWindowPtr();
    inline float getWindowAspect() { return (float) width_ / (float) height_; }
    inline std::pair<uint32_t, uint32_t> extents() { return {width_ ,height_}; }



private:
    void initWindow();
    void createWindow();
    void initGUI();
    void SetWindowTitle(std::string msg = "");


    int width_{WIDTH};
    int height_{HEIGTH};
    std::string windowName_ = {};
    EngineType  engineType;
    ngn::MultiplatformInput *Input;

    bool is_resized = false;

    GLFWwindow* window_ = nullptr;
};
