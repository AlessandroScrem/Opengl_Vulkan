#pragma once
#include "VulkanDevice.hpp"

class VulkanSwapchain
{
public:

    VulkanSwapchain(VulkanDevice &device, Window &window);
     ~VulkanSwapchain();

    VulkanSwapchain(const VulkanSwapchain &) = delete;
    void operator=(const VulkanSwapchain &) = delete;

    // used by VulkanPipeline , VulkanCommandBuffer
    VkExtent2D getExtent(){ return swapChainExtent;}
    VkRenderPass getRenderpass() { return renderPass; }

    // used by VulkanCommandBuffer
    size_t getFramebuffersSize() { return swapChainFramebuffers.size(); }
    VkFramebuffer getFramebuffer(size_t index) { return swapChainFramebuffers[index];}
    
    // used by VulkanImage
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    // used by VulkanEngine
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

    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;

    AllocatedImage depthImage;
    VkImageView depthImageView;    
};


