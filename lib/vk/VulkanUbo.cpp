#include "VulkanUbo.hpp"

//lib
#include <glm/gtc/type_ptr.hpp>

VulkanUbo::VulkanUbo(VulkanDevice &device, VulkanSwapchain &swapchain) : device{device}, swapchain{swapchain}
{
    SPDLOG_DEBUG("constructor");
    createUniformBuffers();
}

VulkanUbo::~VulkanUbo()
{
    SPDLOG_DEBUG("destructor");
    cleanupUniformBuffers();
}

// Necessita
// swapchain.getSwapchianImageSize()
void VulkanUbo::cleanupUniformBuffers() 
{   
    SPDLOG_TRACE("cleanupUniformBuffers");

    auto swapchainImages =  swapchain.getSwapchianImageSize();
    for (size_t i = 0; i < swapchainImages ; i++) {
        vkDestroyBuffer(device.getDevice(), uniformBuffers[i], nullptr);
        vkFreeMemory(device.getDevice(), uniformBuffersMemory[i], nullptr);
    }
}


// Necessita
// swapchain.getSwapchianImageSize()
void VulkanUbo::createUniformBuffers() 
{
    SPDLOG_TRACE("createUniformBuffers");

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    auto swapchainImages =  swapchain.getSwapchianImageSize();
    uniformBuffers.resize(swapchainImages);
    uniformBuffersMemory.resize(swapchainImages);

    for (size_t i = 0; i < swapchainImages; i++) {
        device.createBuffer(bufferSize, 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            uniformBuffers[i], 
            uniformBuffersMemory[i]);
        }
}

// called by VulkanEngine::drawframe()
void VulkanUbo::bind(uint32_t currentImage) 
{
    void* data;
    vkMapMemory(device.getDevice(), uniformBuffersMemory[currentImage], 0, sizeof(UniformBufferObject), 0, &data);
        memcpy(data, glm::value_ptr(model), sizeof(UniformBufferObject));
    vkUnmapMemory(device.getDevice(), uniformBuffersMemory[currentImage]);
}
