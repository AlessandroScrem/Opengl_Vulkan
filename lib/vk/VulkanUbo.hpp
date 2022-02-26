#pragma once
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
//common lib
#include <vertex.h>

class VulkanUbo : public  UniformBufferObject
{
public:
    VulkanUbo(VulkanDevice &device, VulkanSwapchain &swapchian);
    ~VulkanUbo();
    void createUniformBuffers();
    void cleanupUniformBuffers();
    void bind(uint32_t currentImage);

    const std::vector<VkBuffer>& getUniformBuffers(){return uniformBuffers;}
private:

    VulkanDevice &device;
    VulkanSwapchain &swapchain;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
};
