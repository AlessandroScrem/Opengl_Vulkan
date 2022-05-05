#pragma once
#include "VulkanDevice.hpp"

class VulkanSwapchain
{
public:

    VulkanSwapchain(VulkanDevice &device, Window &window);
     ~VulkanSwapchain();

    VulkanSwapchain(const VulkanSwapchain &) = delete;
    void operator=(const VulkanSwapchain &) = delete;

    // getters
    VkExtent2D getExtent(){ return swapChainExtent;}
    VkRenderPass getRenderpass() { return renderPass; }
    VkFramebuffer getFramebuffer(size_t index) { return swapChainFramebuffers[index];}
    VkSwapchainKHR getSwapchain() { return swapChain; }
    size_t getSwapchianImageSize() { return swapChainImages.size(); }

    void cleanupSwapChain();
    void createAllSwapchian();

private:
    void createSwapchain();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void createColorResources();
    void createDepthResources();


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

    VkImageView colorImageView;
    VkImageView depthImageView;    

    AllocatedImage colorImage;
    AllocatedImage depthImage;
};


