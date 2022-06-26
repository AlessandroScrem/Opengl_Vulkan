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

namespace vks
{	
	/**
	* @brief Encapsulates access to a Vulkan buffer backed up by device memory
	* @note To be filled by an external source like the VulkanDevice
	*/
	struct Buffer
	{
		VkDevice device;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo descriptor;
		VkDeviceSize size = 0;
		VkDeviceSize alignment = 0;
		void* mapped = nullptr;
		/** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
		VkBufferUsageFlags usageFlags;
		/** @brief Memory property flags to be filled by external source at buffer creation (to query at some later point) */
		VkMemoryPropertyFlags memoryPropertyFlags;
		VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void unmap();
		VkResult bind(VkDeviceSize offset = 0);
		void setupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void copyTo(void* data, VkDeviceSize size);
		VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void destroy();
	};
}

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
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    const VkSampleCountFlagBits getMsaaSamples() { return _msaaSamples; }
    void setMsaaValue(VkSampleCountFlagBits value); 

    VkCommandPool getDeafaultCommadPool() { return defaultcommandPool; }
    VkPhysicalDeviceProperties getPhysicalDeviceProperties() {return _physicalDeviceProperties;}

    VkResult createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, 
            vks::Buffer *buffer, VkDeviceSize size, void *data = nullptr);


    void createVmaBuffer(        
        VkBufferCreateInfo &bufferInfo, VmaAllocationCreateInfo &vmaallocInfo, 
        VkBuffer &dest_buffer,VmaAllocation &allocation, const void *src_buffer, size_t buffersize);
    void mapVmaBuffer(VmaAllocation &allocation, const void *src_buffer, size_t buffersize);
    void flushVmaAllocation(VmaAllocation &allocation, size_t offset, size_t buffersize);
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

    void init();
    void cleanup();

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createVulkanAllocator();
    void createDefaultCommandPool();


    std::vector<const char*> getRequiredExtensions();
    VkSampleCountFlagBits getMaxUsableSampleCount();

    bool isDeviceSuitable(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

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
    VkDevice logicalDevice;
    VkPhysicalDeviceProperties _physicalDeviceProperties;

    VkSampleCountFlagBits _msaaSamples;
    
    //vma lib allocator
    VmaAllocator _allocator; 

    VkCommandPool defaultcommandPool;
    
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkDebugUtilsMessengerEXT debugMessenger; 
 
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation"};

    // Opengl compatible Viewport (SashaWillems)
    // needs: VK_KHR_MAINTENANCE1_EXTENSION_NAME extension 
    // TODO remove unused extension
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME};
};
