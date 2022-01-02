#pragma once
#include "Engine.hpp"
#include "common/Window.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanCommandBuffer.hpp"


class VulkanEngine : public Engine
{
public:
    VulkanEngine();
    ~VulkanEngine();

    void run() override;

private:
    void drawFrame();

    Window window{EngineType::Vulkan};
    VulkanDevice vkdevice{window};
    VulkanSwapchain swapchain{vkdevice, window};
    VulkanPipeline pipeline{vkdevice, swapchain};
    VulkanCommandBuffer commandBuffer{vkdevice, swapchain, pipeline};
};


