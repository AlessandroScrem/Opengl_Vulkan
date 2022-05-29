#include "VulkanDevice.hpp"
#include "vk_initializers.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
//common lib
#include <Window.hpp>
//std
#include <set>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
    }
}

VulkanDevice::VulkanDevice(Window &window) : window{window}
{
    SPDLOG_DEBUG("constructor");

    createInstance();
    SPDLOG_TRACE("createInstance");
    setupDebugMessenger();
    SPDLOG_TRACE("setupDebugMessenger");
    createSurface();
    SPDLOG_TRACE("createSurface");
    pickPhysicalDevice();
    SPDLOG_TRACE("pickPhysicalDevice");
    createLogicalDevice();
    SPDLOG_TRACE("createLogicalDevice");

    createVulkanAllocator();
    SPDLOG_TRACE("createVulkanAllocator");

    createDefaultCommandPool();
    SPDLOG_TRACE("createDefaultCommandPool");

    setMsaaValue(VK_SAMPLE_COUNT_2_BIT);
}

VulkanDevice::~VulkanDevice() 
{
    SPDLOG_DEBUG("destructor");

    //make sure the gpu has stopped doing its things
	vkDeviceWaitIdle(logicalDevice);

    vkDestroyCommandPool(logicalDevice, defaultcommandPool, nullptr); 
    SPDLOG_TRACE("vkDestroyCommandPool");

    vmaDestroyAllocator(_allocator);
    SPDLOG_TRACE("vmaDestroyAllocator");

    vkDestroyDevice(logicalDevice, nullptr);
    SPDLOG_TRACE("vkDestroyDevice");
 
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        SPDLOG_TRACE("DestroyDebugUtilsMessengerEXT");
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    SPDLOG_TRACE("vkDestroySurfaceKHR");
    vkDestroyInstance(instance, nullptr);
    SPDLOG_TRACE("vkDestroyInstance");
}

void VulkanDevice::createInstance() 
{
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    
}
void VulkanDevice::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);  
    }

    // Specifying used device features
    // Right now we don’t need anything special, so we can simply define it and leave everything to VK_FALSE
    VkPhysicalDeviceFeatures deviceFeatures{};

    // Creating the logical device
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures; 
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    } 

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
    // Retrieving queue handles
    // Because we’re only creating a single queue from this family, we’ll simply use index 0.
    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}

void VulkanDevice::createVulkanAllocator()
{
    //initialize the memory allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = physicalDevice;
    allocatorInfo.device = logicalDevice;
    allocatorInfo.instance = instance;
    vmaCreateAllocator(&allocatorInfo, &_allocator);
}

void VulkanDevice::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            msaaSamples = maxMsaaSamples = getMaxUsableSampleCount();
            setMsaaValue(maxMsaaSamples); // set to max
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    } 
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device) 
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    // check if anisotropic filtering feature is available:
    // feature if requested from texture sampler
    // otherwise we have to disable the feature in texture sampler
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device) 
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // Logic to find queue family indices to populate struct with
    // We need to find at least one queue family that supports both: VK_QUEUE_GRAPHICS_BIT & supporting presentation.
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        // looking for drawing commands support
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        // looking for presentation support
        if (presentSupport) {
            indices.presentFamily = i;
        }

        // if found: early exit
        if (indices.isComplete()) {
            break;
        }

        i++;
    }
    return indices;
}

bool VulkanDevice::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
        //SPDLOG_TRACE("availableExtension = {}", extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device) 
{
    // There are basically three kinds of properties we need to check:
    // 1) Basic surface capabilities (min/max number of images in swap chain, min/max width and height of images)
    // 2) Surface formats (pixel format, color space)
    // 3) Available presentation modes
    
    SwapChainSupportDetails details;

    // 1) Basic surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    // 2) Surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        #ifndef NDEBUG
            //SPDLOG_TRACE(" Found # {} SurfaceFormats", formatCount);
            for(const auto sfmt : details.formats ){
                //SPDLOG_TRACE(" Format {} Colorspace {} ", sfmt.format , sfmt.colorSpace) ;
            }
        #endif
    }


    // 3) Available presentation modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

std::vector<const char*> 
VulkanDevice::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void VulkanDevice::setupDebugMessenger() 
{
     if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);
  
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
    }  
}


void VulkanDevice::createSurface() 
{
     if (glfwCreateWindowSurface(instance, window.getWindowPtr(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void VulkanDevice::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL 
VulkanDevice::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) 
{
    if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){
        spdlog::error("validation layer: {}" ,pCallbackData->pMessage);
    }
    return VK_FALSE;
}

// Helper function ?
/**
 * @brief Find i there is a suitable momory type in the device
 * 
 * @param typeFilter : bit field of the memory types that are suitable for the buffer
 * @param properties : bitmask type of special features of the memory we are looking for
 * @return uint32_t  : return found memoryTypeIndex, otherwise we throw an exception. 
 */
uint32_t VulkanDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) 
{
    // The VkPhysicalDeviceMemoryProperties structure has two arrays memoryTypes and memoryHeaps. 
    // Memory heaps are distinct memory resources like dedicated VRAM and swap space in RAM for when VRAM runs out.
    // The different types of memory exist within these heaps. 
    // Right now we’ll only concern ourselves with the type of memory and not the heap it comes from, 
    //   but you can imagine that this can affect performance.
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

/*  
    It should be noted that in a real world application,
    you’re not supposed to actually call vkAllocateMemory for every individual buffer. 
    The maximum number of simultaneous memory allocations is limited by the maxMemoryAllocationCount physical device limit, 
    which may be as low as 4096 even on high end hardware like an NVIDIA GTX 1080. 
    The right way to allocate memory for a large number of objects at the same time is to create a custom allocator 
    that splits up a single allocation among many different objects by using the offset parameters that we’ve seen in many functions.

    You can either implement such an allocator yourself, 
    or use the VulkanMemoryAllocator library provided by the GPUOpen initiative.
*/ 

/**
 * @brief Create Buffer Memory from Device
 * 
 * @param size : is the size in bytes of the buffer to be created.
 * @param usage: is a bitmask of VkBufferUsageFlagBits specifying allowed usages of the buffer. 
 * @param properties : (VkMemoryPropertyFlags) is a bitmask type for setting a mask of zero or more
 * @param buffer        : the buffer we need to create 
 * @param bufferMemory  : the momory we need to associated with the buffer 
 */
void VulkanDevice::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
                VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) 
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);
    // The VkMemoryRequirements struct has three fields:

    // size:           The size of the required amount of memory in bytes, may differ from bufferInfo.size.
    // alignment:      The offset in bytes where the buffer begins in the allocated region of memory, 
    //                     depends on bufferInfo.usage and bufferInfo.flags.
    // memoryTypeBits: Bit field of the memory types that are suitable for the buffer.

    // Memory allocation
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    //  Associate Device memory with the buffer
    // The first three parameters are self-explanatory and 
    // the fourth parameter is the offset within the region of memory. 
    // Since this memory is allocated specifically for this the vertex buffer, the offset is simply 0. 
    // If the offset is non-zero, then it is required to be divisible by memRequirements.alignment.
    vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
    
}

void VulkanDevice::createCommandPool(VkCommandPool *pool) 
{
    uint32_t queueFamilyIndex = getQueueFamiliesIndices().graphicsFamily.value();
    VkCommandPoolCreateInfo poolInfo = vkinit::command_pool_create_info(
        queueFamilyIndex,VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
      
    VK_CHECK_RESULT(vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, pool) );

}

void VulkanDevice::createDefaultCommandPool()
{
    createCommandPool(&defaultcommandPool);   
}


void VulkanDevice::createVmaBuffer(
        VkBufferCreateInfo &bufferInfo, VmaAllocationCreateInfo &vmaallocInfo, 
        VkBuffer &dest_buffer,VmaAllocation &allocation, const void *src_buffer, size_t buffersize)
{
    VK_CHECK_RESULT(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo, &dest_buffer, &allocation, nullptr) );

    //copy  data
    void* data;
	vmaMapMemory(_allocator, allocation, &data);

	    memcpy(data, src_buffer, buffersize);

	vmaUnmapMemory(_allocator, allocation);
}

void VulkanDevice::destroyVmaBuffer(VkBuffer &buffer,VmaAllocation &allocation)
{
    vmaDestroyBuffer(_allocator, buffer, allocation);
}


 VkFormat VulkanDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
} 

VkFormat VulkanDevice::findDepthFormat() {
    return findSupportedFormat( 
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

// kind of helper function
void VulkanDevice::createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                    VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, 
                    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
                    VkImage& image, VkDeviceMemory& imageMemory) 
{
    VkExtent3D extent = {width, height, 1} ;
    VkImageCreateInfo imageInfo = vkinit::image_create_info(format, usage, extent, numSamples, mipLevels);
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(logicalDevice, image, imageMemory, 0);
} 

// kind of helper function
void VulkanDevice::createVmaImage(VkImageCreateInfo &imageInfo, VmaAllocationCreateInfo &vmaallocInfo, VkImage &dest_image, VmaAllocation &allocation)   
{
    VK_CHECK_RESULT(vmaCreateImage(_allocator, &imageInfo, &vmaallocInfo, &dest_image, &allocation, nullptr) );
}

void VulkanDevice::destroyVmaImage(VkImage &image, VmaAllocation &allocation)
{
    vmaDestroyImage(_allocator, image, allocation);
}

VkSampleCountFlagBits VulkanDevice::getMaxUsableSampleCount() {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

void VulkanDevice::setMsaaValue(VkSampleCountFlagBits value){
    if(value <= maxMsaaSamples){
        msaaSamples = value;
        spdlog::info("mssaaSamples = {}", msaaSamples);
    }
}

// The function involves recording and executing a command buffer 
// 
VkCommandBuffer VulkanDevice::beginSingleTimeCommands() 
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = defaultcommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;   
}

void VulkanDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer) 
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(logicalDevice, defaultcommandPool, 1, &commandBuffer);
}