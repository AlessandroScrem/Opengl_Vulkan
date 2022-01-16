#include "VulkanSwapchain.hpp"

// std
#include <cstdint> // Necessary for UINT32_MAX
#include <algorithm> // Necessary for std::min/std::max


VulkanSwapchain::VulkanSwapchain(VulkanDevice &device, Window &window) 
    : device{device}
    , window{window}
{ 
    SPDLOG_TRACE("constructor");

    createAllSwapchian();
}


VulkanSwapchain::~VulkanSwapchain()
{  
    SPDLOG_TRACE("destructor"); 
    cleanupSwapChain();
    SPDLOG_TRACE("cleanupSwapChain");
}  

void VulkanSwapchain::createAllSwapchian()
{
    SPDLOG_TRACE("createAllSwapchian");
    createSwapchain();
    createImageViews();
    createRenderPass();
    createDepthResources();
    createFramebuffers();
}

void VulkanSwapchain::cleanupSwapChain() 
{
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device.getDevice(), framebuffer, nullptr);
    }
    SPDLOG_TRACE("vkDestroyFramebuffer");

    vkDestroyImageView(device.getDevice(), depthImageView, nullptr);
    vkDestroyImage(device.getDevice(), depthImage, nullptr);
    vkFreeMemory(device.getDevice(), depthImageMemory, nullptr);
    SPDLOG_TRACE("vkDestroy DepthResources");


    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device.getDevice(), imageView, nullptr);
    }
    SPDLOG_TRACE("vkDestroyImageView");


    vkDestroyRenderPass(device.getDevice(), renderPass, nullptr);
    SPDLOG_TRACE("vkDestroyRenderPass");

    vkDestroySwapchainKHR(device.getDevice(), swapChain, nullptr);
    SPDLOG_TRACE("vkDestroySwapchainKHR");

}


void VulkanSwapchain::createSwapchain()
 {
    SPDLOG_TRACE("createSwapchain");

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

    // TODO: maybe move Images creation outside class?
    // get swapchain images number
    vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    
    // get swapchain images to vector
    vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}


// 1) Surface format (color depth)
VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
    // We’ll use SRGB olor space 
    for (const auto& availableFormat : availableFormats) {
        // FIXME change from SRGB TO UNORM to make similar to Opengl apparence 
        //if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
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
    if (capabilities.currentExtent.width != UINT32_MAX && !window.framebufferResized()) {
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


VkImageView VulkanSwapchain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device.getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void VulkanSwapchain::createImageViews() 
{
    SPDLOG_TRACE("createImageViews");

    swapChainImageViews.resize(swapChainImages.size());
    const uint32_t mipmap_one = 1;

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
                swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, mipmap_one);
            }    
}

void VulkanSwapchain::createRenderPass() 
{
    SPDLOG_TRACE("createRenderPass");

    // color Attachment description
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // depth Attachment description
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = device.findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // color Attachment references
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // depth Attachment references
    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;   
    
    // Subpasses
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    // Subpass dependencies
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    // Render pass
    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device.getDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

}

void VulkanSwapchain::createFramebuffers() 
{   
    SPDLOG_TRACE("createFramebuffers");

    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            std::array<VkImageView, 2> attachments = {
                swapChainImageViews[i],
                depthImageView
            };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device.getDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void VulkanSwapchain::createDepthResources() 
{
    SPDLOG_TRACE("createDepthResources");

    VkFormat depthFormat = device.findDepthFormat();
    const uint32_t mipmap_one = 1;

    device.createImage(swapChainExtent.width,  swapChainExtent.height, mipmap_one, 
                depthFormat, 
                VK_IMAGE_TILING_OPTIMAL, 
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                depthImage, 
                depthImageMemory
                );
    depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, mipmap_one);
}

