#include "VulkanSwapchain.hpp"

// std
#include <iostream>
#include <cstdint> // Necessary for UINT32_MAX
#include <algorithm> // Necessary for std::min/std::max


VulkanSwapchain::VulkanSwapchain(VulkanDevice &device, Window &window) 
    : device{device}
    , window{window}
{ 
    std::cout << "VulkanSwapchain  constructor\n";
    createSwapchain();
}


VulkanSwapchain::~VulkanSwapchain()
{ 
    std::cout << "VulkanSwapchain  destructor\n"; 
    vkDestroySwapchainKHR(device.getDevice(), swapChain, nullptr);
}

void VulkanSwapchain::createSwapchain()
 {
    // We’ll now find the right settings for the best possible swap chain. 
    // There are three types of settings to determine:

    SwapChainSupportDetails swapChainSupport = device.getSwapChainSupport();

    // 1) Surface format (color depth)
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    // 2) Presentation mode (conditions for “swapping” images to the screen)
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    // 3) Swap extent (resolution of images in swap chain)
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    // we also have to decide how many images we would like to have in the swap chain. 
    // The implementation specifies the minimum number that it requires to function:
    // It's recommended to request at least one more image than the minimum:
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    // We should also make sure to not exceed the maximum number of images
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = device.getSurface();
    // After specifying which surface the swap chain should be tied to, the details of the swap chain images are specified:

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = device.findPhysicalQueueFamilies();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device.getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
}

// 1) Surface format (color depth)
VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
    // We’ll use SRGB olor space 
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    // If fails,
    // it’s okay to just settle with the first format that is specified.
    return availableFormats[0];
}

// 2) Presentation mode (conditions for “swapping” images to the screen)
VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
{
    // VK_PRESENT_MODE_IMMEDIATE_KHR: 
    //  Images submitted by your application are transferred to the screen right away, which may result in tearing.
    
    // VK_PRESENT_MODE_FIFO_KHR: 
    //  The swap chain is a queue where the display takes an image from the front of the queue when the display is refreshed and the program inserts rendered images at the back of the queue. 
    //  If the queue is full then the program has to wait. This is most similar to vertical sync as found in modern games. 
    //  The moment that the display is refreshed is known as “vertical blank”.

    // VK_PRESENT_MODE_FIFO_RELAXED_KHR: 
    //  This mode only differs from the previous one if the application is late and the queue was empty at the last vertical blank. 
    //  Instead of waiting for the next vertical blank, the image is transferred right away when it finally arrives. 
    //  This may result in visible tearing.
    
    // VK_PRESENT_MODE_MAILBOX_KHR: 
    //  This is another variation of the second mode. Instead of blocking the application when the queue is full, 
    //  the images that are already queued are simply replaced with the newer ones. 
    //  This mode can be used to render frames as fast as possible while still avoiding tearing, resulting in fewer latency issues than standard vertical sync. 
    //  This is commonly known as “triple buffering”, although the existence of three buffers alone does not necessarily mean that the framerate is unlocked.
    
    // loking for VK_PRESENT_MODE_MAILBOX_KHR ,is a very nice trade-off if energy usage is not a concern. 
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    // Only the VK_PRESENT_MODE_FIFO_KHR mode is guaranteed to be available,
    return VK_PRESENT_MODE_FIFO_KHR;
}




// 3) Swap extent (resolution of images in swap chain)
VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
{    
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {

        auto[width, height] = window.GetWindowExtents();
        

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        // The max and min functions are used here to clamp the value of WIDTH and HEIGHT 
        // between the allowed minimum and maximum extents that are supported by the implementation.
        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}
