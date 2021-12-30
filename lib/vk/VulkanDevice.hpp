#pragma once
#include "common/Window.hpp"

//lib
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//std
#include <vector>
#include <optional>


struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily; 
    
    bool isComplete() {
        return graphicsFamily.has_value();
    }
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
private:
    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();

    std::vector<const char*> getRequiredExtensions();

    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    //validation layer helper functions
    bool checkValidationLayerSupport();
    void setupDebugMessenger();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    VkInstance instance;
    VkDevice device;
    VkQueue graphicsQueue;

    VkDebugUtilsMessengerEXT debugMessenger; 
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    Window &window;
    
 
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation"};
};
