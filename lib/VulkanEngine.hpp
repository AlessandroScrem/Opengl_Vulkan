#pragma once
#include "Engine.hpp"
#include "Window.hpp"
#include "VulkanDevice.hpp"


class VulkanEngine : public Engine
{
public:
    VulkanEngine(){    std::cout << "VulkanEngine  constructor\n";}
    ~VulkanEngine();

    void run();
private:
    void mainLoop();

    Window window{EngineType::Vulkan};
    VulkanDevice vkdevice{window};
};


