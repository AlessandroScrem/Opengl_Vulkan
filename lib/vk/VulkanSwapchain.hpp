#pragma once
#include "VulkanDevice.hpp"
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

    // used by VulkanPipeline
    VkExtent2D getExtent(){ return swapChainExtent;}
    VkRenderPass getRenderpass() { return renderPass; }

private:
    void createSwapchain();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


    Window &window;
    VulkanDevice &device;

    VkSwapchainKHR swapChain;
    VkExtent2D swapChainExtent;

    VkRenderPass renderPass;

    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    VkFormat swapChainImageFormat;

    std::vector<VkFramebuffer> swapChainFramebuffers;
};


