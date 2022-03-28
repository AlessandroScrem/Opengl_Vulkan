#pragma once
//common lib
#include <Window.hpp>
//lib
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vktypes.h"
//std
#include <vector>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily; 
    
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
}; 

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


class VulkanDevice
{    
public:
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif  

    VulkanDevice(Window &window);
    ~VulkanDevice();

    // Not copyable or movable
    VulkanDevice(const VulkanDevice &) = delete;
    void operator=(const VulkanDevice &) = delete;
    VulkanDevice(VulkanDevice &&) = delete;
    VulkanDevice &operator=(VulkanDevice &&) = delete;

    // used by VulkanSwapchain , VulkanCommandBuffer
    SwapChainSupportDetails getSwapChainSupport(){ return querySwapChainSupport(physicalDevice);}
    QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
    VkSurfaceKHR getSurface(){ return surface;}
    VkDevice getDevice() { return logicalDevice; }
    
    const VkSampleCountFlagBits getMsaaSamples() { return msaaSamples; }

    // used by VulkanVertexBuffer
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
                VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void createVmaBuffer(        
        VkBufferCreateInfo &bufferInfo, VmaAllocationCreateInfo &vmaallocInfo, 
        VkBuffer &dest_buffer,VmaAllocation &allocation, const void *src_buffer, size_t buffersize);
    void destroyVmaBuffer(VkBuffer &buffer,VmaAllocation &allocation);

    void createVmaImage(VkImageCreateInfo &imageInfo, VmaAllocationCreateInfo &vmaallocInfo, VkImage &dest_image, VmaAllocation &allocation);  
    void destroyVmaImage(VkImage &image, VmaAllocation &allocation);

    // used by VulkanEngine , VulkanImage
    VkCommandPool getCommadPool() { return commandPool; }


    VkQueue getGraphicsQueue() {return graphicsQueue; }
    VkQueue getPresentQueue() {return presentQueue; }
    VkFormat findDepthFormat();
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                    VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, 
                    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
                    VkImage& image, VkDeviceMemory& imageMemory);
    
    // used by VulkanImage
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    void GetPhysicalDeviceProperties(VkPhysicalDeviceProperties &properties);
    void GetPhysicalDeviceFormatProperties(const VkFormat imageFormat, VkFormatProperties &formatProperties); 



private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createVulkanAllocator();

    void createCommandPool();
    

    std::vector<const char*> getRequiredExtensions();
    VkSampleCountFlagBits getMaxUsableSampleCount(); 
    void setMsaaValue(VkSampleCountFlagBits value); 

    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    bool checkValidationLayerSupport();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    //validation layer helper functions
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    
    Window &window;
    VkInstance instance;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSampleCountFlagBits maxMsaaSamples, msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    VkDevice logicalDevice;

    VmaAllocator _allocator; //vma lib allocator

    VkCommandPool commandPool;
    
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkDebugUtilsMessengerEXT debugMessenger; 
 
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation"};

    // Opengl compatible Viewport (SashaWillems)
    // needs: VK_KHR_MAINTENANCE1_EXTENSION_NAME extension 
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME};
};
