#pragma once

#include "VulkanDevice.hpp"

class VulkanSwapchain;
struct GLFWwindow;

class VulkanUIOverlay
{
public:
    GLFWwindow *windowPtr;
    VulkanDevice *device;
    VulkanSwapchain *swapchain;

    void init();

    void newFrame();
    void draw(VkCommandBuffer cmd);
    void cleanup();

private:
    VkDescriptorPool _gui_DescriptorPool = VK_NULL_HANDLE; 
};
