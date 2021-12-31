#pragma once
#include "VulkanDevice.hpp"
//#include "..\common\Window.hpp"
#include <vulkan/vulkan.hpp>

//std
#include <iostream>
#include <vector>


class VulkanSwapchain
{
public:

    VulkanSwapchain(VulkanDevice &device, Window &window);
     ~VulkanSwapchain();

    VulkanSwapchain(const VulkanSwapchain &) = delete;
    void operator=(const VulkanSwapchain &) = delete;

    void createSwapchain();

private:
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    Window &window;
    VulkanDevice &device;

    VkSwapchainKHR swapChain;
};


