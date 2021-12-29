#pragma once

//std
#include <memory>
#include <iostream>

enum class WindowType
{
    Opengl,
    Vulkan
};

class Window
{    
public:
    Window(){ std::cout << "Window  constructor\n";}
    virtual ~Window(){std::cout << "Window  destructor\n";}
       
    virtual void run() = 0;

   static std::unique_ptr<Window> create(WindowType type);
private:
};

