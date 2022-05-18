#pragma once
//common lib
#include <mytypes.hpp>
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

class Window;

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

    // some getters
    SwapChainSupportDetails getSwapChainSupport(){ return querySwapChainSupport(physicalDevice);}
    QueueFamilyIndices getQueueFamiliesIndices() { return findQueueFamilies(physicalDevice); }
    VkSurfaceKHR getSurface(){ return surface;}
    VkDevice getDevice() { return logicalDevice; }
    VkPhysicalDevice getPhysicalDevice() {return physicalDevice; }
    VkInstance getInstance() {return instance; }
    VkQueue getGraphicsQueue() {return graphicsQueue; }
    VkQueue getPresentQueue() {return presentQueue; } 
    VkFormat getDepthFormat() {return findDepthFormat(); }
    const VkSampleCountFlagBits getMsaaSamples() { return msaaSamples; }

    VkCommandPool getDeafaultCommadPool() { return defaultcommandPool; }   


    // used by VulkanVertexBuffer
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
                VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);


    void createVmaBuffer(        
        VkBufferCreateInfo &bufferInfo, VmaAllocationCreateInfo &vmaallocInfo, 
        VkBuffer &dest_buffer,VmaAllocation &allocation, const void *src_buffer, size_t buffersize);
    void destroyVmaBuffer(VkBuffer &buffer,VmaAllocation &allocation);

    void createVmaImage(VkImageCreateInfo &imageInfo, VmaAllocationCreateInfo &vmaallocInfo, VkImage &dest_image, VmaAllocation &allocation);  
    void destroyVmaImage(VkImage &image, VmaAllocation &allocation);

    void createCommandPool(VkCommandPool *pool);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                    VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, 
                    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
                    VkImage& image, VkDeviceMemory& imageMemory);

    VkCommandBuffer beginSingleTimeCommands(); 
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createVulkanAllocator();
    void createDefaultCommandPool();

    void setMsaaValue(VkSampleCountFlagBits value); 

    std::vector<const char*> getRequiredExtensions();
    VkSampleCountFlagBits getMaxUsableSampleCount();

    bool isDeviceSuitable(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();  

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

    VkCommandPool defaultcommandPool;
    
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkDebugUtilsMessengerEXT debugMessenger; 
 
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation"};

    // Opengl compatible Viewport (SashaWillems)
    // needs: VK_KHR_MAINTENANCE1_EXTENSION_NAME extension 
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME};
};
