#pragma once
#include "common/Window.hpp"


//lib
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//std
#include <iostream>
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

    VkCommandPool getCommadPool() { return commandPool; }

    // used by VulkanVertexBuffer
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
                VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    
    // used by VulkanEngine
    VkQueue getGraphicsQueue() {return graphicsQueue; }
    VkQueue getPresentQueue() {return presentQueue; }

    

private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();

    void createCommandPool();
    
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    std::vector<const char*> getRequiredExtensions();

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
    VkDevice logicalDevice;

    VkCommandPool commandPool;
    
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkDebugUtilsMessengerEXT debugMessenger; 
 
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
