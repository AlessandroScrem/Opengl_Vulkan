#pragma once
#include "Engine.hpp"
#include "common/Window.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanPipeline.hpp"


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
    VulkanSwapchain swapchain{vkdevice, window};
    VulkanPipeline pipeline{vkdevice};
};


